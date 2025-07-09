#include <stdio.h>
#include <errno.h>
#include <geometry.h>
#include <bounding-box.h>


#define DEFAULT_PAGE_WIDTH 2400
#define DEFAULT_PAGE_HEIGHT 3400
#define DEFAULT_PAGE_DIM_TOL 0.05
#define OUT_SVG_FILENAME_TEMPLATE "./data/page-%d.svg"
#define MAX_PAGE_COUNT 100


int main(int argc, char **argv) {
    BboxCollection *page_inner_items[MAX_PAGE_COUNT] = {0};
    SvgDocument *page_docs[MAX_PAGE_COUNT] = {0};
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.svg\n", argv[0]);
        goto InvalidArgs;
    }
    char const *src_filename = argv[1];

    SvgDocument *src_doc = bbox_svg_doc_make_from_file(src_filename);
    if (!src_doc) {
        perror("Failed at parsing file");
        goto DocumentParseFailed;
    }

    BboxCollection *all_items = bbox_collection_make_from_doc(src_doc);
    if (!all_items) {
        perror("Failed at extracting bounding box collection");
        goto CollectionMakeFailed;
    }

    BboxCollection *page_items = bbox_collection_filter_by_dimensions(
        all_items,
        (Size2D){DEFAULT_PAGE_WIDTH,DEFAULT_PAGE_HEIGHT},
        DEFAULT_PAGE_DIM_TOL
    );
    if (!page_items) {
        goto FilteredCollectionMakeFailed;
    }

    bbox_collection_sort_dim(page_items);

    unsigned int page_count = bbox_collection_get_count(page_items);
    printf(
        "Obtained %d XML page elements out of %d.\n",
        page_count,
        bbox_collection_get_count(all_items)
    );

    for (unsigned int i=0; i < page_count; i++) {
        BboxItem const * page_item = bbox_collection_getc(page_items, i);
        page_inner_items[i] = \
            bbox_collection_filter_intersecting(all_items, page_item);
        if (!page_inner_items[i]) {
            continue;
        }

        printf(
            "For page %d we have %d elements.\n",
            i+1,
            bbox_collection_get_count(page_inner_items[i])
        );

        page_docs[i] = bbox_svg_doc_make_from_subset(
            src_doc,
            page_inner_items[i]
        );
        bbox_svg_doc_set_viewbox(page_docs[i], page_item->bbox);
    }

    for (unsigned int i=0; i < page_count; i++) {
        char filename[32];
        snprintf(filename, 32, OUT_SVG_FILENAME_TEMPLATE, i);
        bbox_svg_doc_save_file(page_docs[i], filename);
        bbox_svg_doc_destroy(page_docs[i]);
        bbox_collection_destroy(page_inner_items[i]);
    }

    bbox_collection_destroy(page_items);
    bbox_collection_destroy(all_items);
    bbox_svg_doc_destroy(src_doc);
    bbox_clean_up();
    return 0;

FilteredCollectionMakeFailed:
    bbox_collection_destroy(all_items);
CollectionMakeFailed:
    bbox_svg_doc_destroy(src_doc);
    bbox_clean_up();
DocumentParseFailed:
InvalidArgs:
    return 1;
};
