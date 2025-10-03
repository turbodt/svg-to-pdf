#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <geometry.h>
#include <bounding-box.h>
#include "./app_props.h"


#define DEFAULT_PAGE_WIDTH 2400
#define DEFAULT_PAGE_HEIGHT 3400
#define DEFAULT_PAGE_DIM_TOL 0.05
#define MAX_PAGE_COUNT 100
#define POSITON_ARRANGEMENT_TOL 0.05

static int bbox_collection_cmp(BboxItem const *, BboxItem const *);


AppProps props = {
    .page = {
        .size = {
            .width = DEFAULT_PAGE_WIDTH,
            .height = DEFAULT_PAGE_HEIGHT,
        },
        .tol = DEFAULT_PAGE_DIM_TOL,
        .include_containers = 1,
    },
};


int main(int argc, char **argv) {
    if (app_parse_props(argc, argv, &props)) {
        app_print_usage(stderr, argv[0]);
        goto InvalidArgs;
    }

    SvgDocument *src_doc = bbox_svg_doc_make_from_file(props.input.filename);
    if (!src_doc) {
        perror("Failed at parsing file");
        goto DocumentParseFailed;
    }

    BboxCollection *all_items = bbox_collection_make_from_doc(src_doc);
    if (!all_items) {
        perror("Failed at extracting bounding box collection");
        goto CollectionMakeFailed;
    }

    BboxCollection *page_collection = bbox_collection_filter_by_dimensions(
        all_items,
        props.page.size,
        props.page.tol
    );
    if (!page_collection) {
        goto FilteredCollectionMakeFailed;
    }

    bbox_collection_sort(page_collection, &bbox_collection_cmp);

    unsigned int page_count = bbox_collection_get_count(page_collection);
    printf(
        "Obtained %d XML page elements out of %d.\n",
        page_count,
        bbox_collection_get_count(all_items)
    );

    BboxCollection **page_content_collections =
        (BboxCollection **) malloc(sizeof(BboxCollection *)*page_count);
    if (!page_content_collections) {
        goto PageContentCollectionAllocFailed;
    }

    SvgDocument **page_docs =
        (SvgDocument **) malloc(sizeof(SvgDocument *)*page_count);
    if (!page_docs) {
        goto PageDocumentsAllocFailed;
    }

    for (unsigned int i=0; i < page_count; i++) {
        BboxItem const * page_item = bbox_collection_getc(page_collection, i);
        page_content_collections[i] = \
            bbox_collection_filter_intersecting(all_items, page_item);
        if (!page_content_collections[i]) {
            continue;
        }

        if (props.page.include_containers) {
            bbox_collection_append(
                page_content_collections[i],
                page_item->id,
                page_item->bbox
            );
        }

        printf(
            "For page %d we have %d elements.\n",
            i+1,
            bbox_collection_get_count(page_content_collections[i])
        );

        page_docs[i] = bbox_svg_doc_make_from_subset(
            src_doc,
            page_content_collections[i]
        );
        bbox_svg_doc_set_viewbox(page_docs[i], page_item->bbox);
    }

    for (unsigned int i=0; i < page_count; i++) {
        char filename[128];
        snprintf(
            filename,
            sizeof(filename),
            "%s/page-%05i.svg",
            props.output.dirname,
            i+1
        );
        bbox_svg_doc_save_file(page_docs[i], filename);
        bbox_svg_doc_destroy(page_docs[i]);
        bbox_collection_destroy(page_content_collections[i]);
    }

    free(page_docs);
    free(page_content_collections);
    bbox_collection_destroy(page_collection);
    bbox_collection_destroy(all_items);
    bbox_svg_doc_destroy(src_doc);
    bbox_clean_up();
    return 0;
PageDocumentsAllocFailed:
    free(page_content_collections);
PageContentCollectionAllocFailed:
    bbox_collection_destroy(page_collection);
FilteredCollectionMakeFailed:
    bbox_collection_destroy(all_items);
CollectionMakeFailed:
    bbox_svg_doc_destroy(src_doc);
    bbox_clean_up();
DocumentParseFailed:
InvalidArgs:
    return 1;
};


int bbox_collection_cmp(BboxItem const *a, BboxItem const *b) {
    static const float pos_tol = 1 - POSITON_ARRANGEMENT_TOL;
    if (a->bbox.tl.y + pos_tol * props.page.size.height < b->bbox.tl.y) {
        return -1;
    } else if (
        b->bbox.tl.y + pos_tol * props.page.size.height < a->bbox.tl.y
    ) {
        return 1;
    }

    if (a->bbox.tl.x + pos_tol * props.page.size.width < b->bbox.tl.x) {
        return -1;
    } else if (
        b->bbox.tl.x + pos_tol * props.page.size.width < a->bbox.tl.x
    ) {
        return 1;
    }

    if (a->bbox.tl.y < b->bbox.tl.y) {
        return -1;
    } else if (b->bbox.tl.y < a->bbox.tl.y) {
        return 1;
    }

    return a->bbox.tl.x - b->bbox.tl.x;
};

