#include <bounding-box.h>
#include <geometry.h>
#include <pdf-renderer.h>
#include <libxml/tree.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define DEFAULT_PAGE_WIDTH 2400
#define DEFAULT_PAGE_HEIGHT 3400
#define DEFAULT_PAGE_DIM_TOL 0.05
#define POSITION_ARRANGEMENT_TOL 0.30


typedef struct {
    double container_width;
    double container_height;
    int include_containers;
    int merge_duplicated_containers;
    int has_pdf_size;
    double pdf_width;
    double pdf_height;
} SvgToPdfWasmOptions;


typedef struct {
    Size2D container_size;
    int include_containers;
    int merge_duplicated_containers;
    PdfRenderOptions pdf;
} ConvertOptions;


typedef struct {
    SvgDocument *src_doc;
    BboxCollection *all_items;
    BboxCollection *page_collection;
    BboxCollection *not_page_collection;
    BboxCollection **page_content_collections;
    SvgDocument **page_docs;
    unsigned int page_count;
    unsigned int output_page_count;
} PageSet;


static double cmp_page_width = DEFAULT_PAGE_WIDTH;
static double cmp_page_height = DEFAULT_PAGE_HEIGHT;


static ConvertOptions parse_options(SvgToPdfWasmOptions const *options);
static int bbox_collection_cmp(BboxItem const *a, BboxItem const *b);
static int build_pages(char const *svg, uint32_t svg_len, ConvertOptions const *options, PageSet *out);
static void pageset_destroy(PageSet *pages);
static int doc_to_memory(SvgDocument *doc, uint8_t **out_data, uint32_t *out_len);
static int append_u32(uint8_t **data, uint32_t *len, uint32_t *cap, uint32_t value);
static int append_bytes(uint8_t **data, uint32_t *len, uint32_t *cap, uint8_t const *src, uint32_t src_len);


__attribute__((visibility("default")))
void *svg_to_pdf_alloc(uint32_t len) {
    return malloc(len);
}


__attribute__((visibility("default")))
void svg_to_pdf_free(void *ptr) {
    free(ptr);
}


__attribute__((visibility("default")))
uint8_t *svg_to_pdf_from_memory(
    char const *svg,
    uint32_t svg_len,
    SvgToPdfWasmOptions const *options,
    uint32_t *out_pdf_len
) {
    if (!svg || svg_len == 0 || !out_pdf_len) {
        return NULL;
    }
    *out_pdf_len = 0;

    ConvertOptions parsed_options = parse_options(options);
    PageSet pages = {0};
    if (build_pages(svg, svg_len, &parsed_options, &pages)) {
        return NULL;
    }

    PdfPage *pdf_pages = calloc(pages.output_page_count, sizeof(PdfPage));
    if (!pdf_pages) {
        pageset_destroy(&pages);
        return NULL;
    }

    uint8_t *pdf_data = NULL;
    unsigned long pdf_len = 0;
    for (unsigned int i = 0; i < pages.output_page_count; i++) {
        if (pdf_render_svg_doc_to_page(
            bbox_svg_doc_get_xml_doc(pages.page_docs[i]),
            &parsed_options.pdf,
            &pdf_pages[i]
        )) {
            goto Cleanup;
        }
    }

    if (pdf_write_pages_to_memory(
        pdf_pages,
        pages.output_page_count,
        (void **) &pdf_data,
        &pdf_len
    )) {
        pdf_data = NULL;
    } else {
        *out_pdf_len = (uint32_t) pdf_len;
    }

Cleanup:
    for (unsigned int i = 0; i < pages.output_page_count; i++) {
        pdf_page_destroy(&pdf_pages[i]);
    }
    free(pdf_pages);
    pageset_destroy(&pages);
    return pdf_data;
}


__attribute__((visibility("default")))
uint8_t *svg_to_pdf_page_svg_from_memory(
    char const *svg,
    uint32_t svg_len,
    uint32_t page_number,
    SvgToPdfWasmOptions const *options,
    uint32_t *out_svg_len
) {
    if (!svg || svg_len == 0 || page_number == 0 || !out_svg_len) {
        return NULL;
    }
    *out_svg_len = 0;

    ConvertOptions parsed_options = parse_options(options);
    PageSet pages = {0};
    if (build_pages(svg, svg_len, &parsed_options, &pages)) {
        return NULL;
    }

    uint8_t *out = NULL;
    if (page_number <= pages.output_page_count) {
        doc_to_memory(pages.page_docs[page_number - 1], &out, out_svg_len);
    }
    pageset_destroy(&pages);
    return out;
}


__attribute__((visibility("default")))
uint8_t *svg_to_pdf_page_svgs_from_memory(
    char const *svg,
    uint32_t svg_len,
    SvgToPdfWasmOptions const *options,
    uint32_t *out_packed_len
) {
    if (!svg || svg_len == 0 || !out_packed_len) {
        return NULL;
    }
    *out_packed_len = 0;

    ConvertOptions parsed_options = parse_options(options);
    PageSet pages = {0};
    if (build_pages(svg, svg_len, &parsed_options, &pages)) {
        return NULL;
    }

    uint8_t *packed = NULL;
    uint32_t packed_len = 0;
    uint32_t packed_cap = 0;
    if (append_u32(&packed, &packed_len, &packed_cap, pages.output_page_count)) {
        goto Cleanup;
    }

    for (unsigned int i = 0; i < pages.output_page_count; i++) {
        uint8_t *page_data = NULL;
        uint32_t page_len = 0;
        if (doc_to_memory(pages.page_docs[i], &page_data, &page_len)) {
            free(packed);
            packed = NULL;
            packed_len = 0;
            goto Cleanup;
        }
        int err = append_u32(&packed, &packed_len, &packed_cap, page_len)
            || append_bytes(&packed, &packed_len, &packed_cap, page_data, page_len);
        free(page_data);
        if (err) {
            free(packed);
            packed = NULL;
            packed_len = 0;
            goto Cleanup;
        }
    }

Cleanup:
    pageset_destroy(&pages);
    *out_packed_len = packed_len;
    return packed;
}


static ConvertOptions parse_options(SvgToPdfWasmOptions const *options) {
    ConvertOptions parsed = {
        .container_size = {
            .width = DEFAULT_PAGE_WIDTH,
            .height = DEFAULT_PAGE_HEIGHT,
        },
        .include_containers = 1,
        .merge_duplicated_containers = 1,
        .pdf = {0},
    };
    if (!options) {
        return parsed;
    }
    if (options->container_width > 0) {
        parsed.container_size.width = options->container_width;
    }
    if (options->container_height > 0) {
        parsed.container_size.height = options->container_height;
    }
    parsed.include_containers = options->include_containers != 0;
    parsed.merge_duplicated_containers = options->merge_duplicated_containers != 0;
    if (options->has_pdf_size && options->pdf_width > 0 && options->pdf_height > 0) {
        parsed.pdf.has_page_size = 1;
        parsed.pdf.page_size.width = options->pdf_width;
        parsed.pdf.page_size.height = options->pdf_height;
    }
    return parsed;
}


static int build_pages(char const *svg, uint32_t svg_len, ConvertOptions const *options, PageSet *out) {
    out->src_doc = bbox_svg_doc_make_from_memory(svg, svg_len);
    if (!out->src_doc) {
        return 1;
    }

    out->all_items = bbox_collection_make_from_doc(out->src_doc);
    if (!out->all_items) {
        return 1;
    }

    out->page_collection = bbox_collection_filter_by_dimensions(
        out->all_items,
        options->container_size,
        DEFAULT_PAGE_DIM_TOL
    );
    if (!out->page_collection) {
        return 1;
    }

    out->not_page_collection = bbox_collection_filter_by_dimensions_not(
        out->all_items,
        options->container_size,
        DEFAULT_PAGE_DIM_TOL
    );
    if (!out->not_page_collection) {
        return 1;
    }

    cmp_page_width = options->container_size.width;
    cmp_page_height = options->container_size.height;
    bbox_collection_sort(out->page_collection, &bbox_collection_cmp);
    out->page_count = bbox_collection_get_count(out->page_collection);

    if (out->page_count == 0) {
        out->page_count = 1;
        out->output_page_count = 1;
        out->page_docs = calloc(1, sizeof(SvgDocument *));
        if (!out->page_docs) {
            return 1;
        }
        out->page_docs[0] = out->src_doc;
        out->src_doc = NULL;
        return 0;
    }

    out->page_content_collections = calloc(out->page_count, sizeof(BboxCollection *));
    out->page_docs = calloc(out->page_count, sizeof(SvgDocument *));
    if (!out->page_content_collections || !out->page_docs) {
        return 1;
    }

    unsigned int skiped_count = 0;
    for (unsigned int i = 0; i + skiped_count < out->page_count; i++) {
        unsigned int equivalent_page_count = 1;
        BboxItem const *page_item = bbox_collection_getc(
            out->page_collection,
            i + skiped_count
        );
        BboxItem const *next_page_item = bbox_collection_getc(
            out->page_collection,
            i + skiped_count + equivalent_page_count
        );

        while (
            options->merge_duplicated_containers
            && next_page_item
            && bbox_collection_cmp(page_item, next_page_item) == 0
        ) {
            equivalent_page_count++;
            next_page_item = bbox_collection_getc(
                out->page_collection,
                i + skiped_count + equivalent_page_count
            );
        }

        out->page_content_collections[i] = bbox_collection_filter_intersecting(
            out->not_page_collection,
            page_item
        );
        if (!out->page_content_collections[i]) {
            return 1;
        }

        if (options->include_containers) {
            for (unsigned int j = 0; j < equivalent_page_count; j++) {
                BboxItem const *container = bbox_collection_getc(
                    out->page_collection,
                    i + skiped_count + j
                );
                if (bbox_collection_append(
                    out->page_content_collections[i],
                    container->id,
                    container->bbox
                )) {
                    return 1;
                }
            }
        }

        out->page_docs[i] = bbox_svg_doc_make_from_subset(
            out->src_doc,
            out->page_content_collections[i]
        );
        if (!out->page_docs[i]) {
            return 1;
        }
        bbox_svg_doc_set_viewbox(out->page_docs[i], page_item->bbox);
        out->output_page_count = i + 1;
        skiped_count += equivalent_page_count - 1;
    }

    return out->output_page_count == 0;
}


static void pageset_destroy(PageSet *pages) {
    if (pages->page_docs) {
        for (unsigned int i = 0; i < pages->page_count; i++) {
            if (pages->page_docs[i]) {
                bbox_svg_doc_destroy(pages->page_docs[i]);
            }
        }
        free(pages->page_docs);
    }
    if (pages->page_content_collections) {
        for (unsigned int i = 0; i < pages->page_count; i++) {
            if (pages->page_content_collections[i]) {
                bbox_collection_destroy(pages->page_content_collections[i]);
            }
        }
        free(pages->page_content_collections);
    }
    if (pages->not_page_collection) {
        bbox_collection_destroy(pages->not_page_collection);
    }
    if (pages->page_collection) {
        bbox_collection_destroy(pages->page_collection);
    }
    if (pages->all_items) {
        bbox_collection_destroy(pages->all_items);
    }
    if (pages->src_doc) {
        bbox_svg_doc_destroy(pages->src_doc);
    }
    *pages = (PageSet){0};
}


static int doc_to_memory(SvgDocument *doc, uint8_t **out_data, uint32_t *out_len) {
    xmlChar *xml = NULL;
    int len = 0;
    *out_data = NULL;
    *out_len = 0;
    xmlDocDumpMemory(bbox_svg_doc_get_xml_doc(doc), &xml, &len);
    if (!xml || len <= 0) {
        if (xml) {
            xmlFree(xml);
        }
        return 1;
    }
    uint8_t *copy = malloc((size_t) len);
    if (!copy) {
        xmlFree(xml);
        return 1;
    }
    memcpy(copy, xml, (size_t) len);
    xmlFree(xml);
    *out_data = copy;
    *out_len = (uint32_t) len;
    return 0;
}


static int append_u32(uint8_t **data, uint32_t *len, uint32_t *cap, uint32_t value) {
    uint8_t bytes[4] = {
        (uint8_t)(value & 0xff),
        (uint8_t)((value >> 8) & 0xff),
        (uint8_t)((value >> 16) & 0xff),
        (uint8_t)((value >> 24) & 0xff),
    };
    return append_bytes(data, len, cap, bytes, 4);
}


static int append_bytes(uint8_t **data, uint32_t *len, uint32_t *cap, uint8_t const *src, uint32_t src_len) {
    uint32_t target = *len + src_len;
    if (target < *len) {
        return 1;
    }
    if (target > *cap) {
        uint32_t next_cap = *cap ? *cap : 4096;
        while (next_cap < target) {
            next_cap *= 2;
            if (next_cap < *cap) {
                return 1;
            }
        }
        uint8_t *next = realloc(*data, next_cap);
        if (!next) {
            return 1;
        }
        *data = next;
        *cap = next_cap;
    }
    memcpy(*data + *len, src, src_len);
    *len = target;
    return 0;
}


static int bbox_collection_cmp(BboxItem const *a, BboxItem const *b) {
    static const float pos_tol = 1 - POSITION_ARRANGEMENT_TOL;
    if (a->bbox.tl.y + pos_tol * cmp_page_height < b->bbox.tl.y) {
        return -1;
    } else if (b->bbox.tl.y + pos_tol * cmp_page_height < a->bbox.tl.y) {
        return 1;
    }

    if (a->bbox.tl.x + pos_tol * cmp_page_width < b->bbox.tl.x) {
        return -1;
    } else if (b->bbox.tl.x + pos_tol * cmp_page_width < a->bbox.tl.x) {
        return 1;
    }

    return 0;
}
