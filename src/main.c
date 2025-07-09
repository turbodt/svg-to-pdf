#include <stdio.h>
#include <errno.h>
#include "./xml-bbox-extractor.h"
#include <geometry.h>


#define DEFAULT_PAGE_WIDTH 2400
#define DEFAULT_PAGE_HEIGHT 3400
#define DEFAULT_PAGE_DIM_TOL 0.05


static XmlBboxCollection *xml_bbox_collection_filter_by_dim(
    XmlBboxCollection const *collection,
    Size2D target_size,
    float tolerance
);


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.svg\n", argv[0]);
        goto InvalidArgs;
    }

    xmlDoc *doc = xmlReadFile(argv[1], NULL, 0);
    if (!doc) {
        perror("Failed at parsing file");
        goto DocumentParseFailed;
    }

    XmlBboxCollection *collection = xml_bbox_collection_make_from_doc(doc);
    if (!collection) {
        perror("Failed at extracting bounding box collection");
        goto CollectionMakeFailed;
    }
    XmlBboxCollection *filtered_coll = xml_bbox_collection_filter_by_dim(
        collection,
        (Size2D){DEFAULT_PAGE_WIDTH,DEFAULT_PAGE_HEIGHT},
        DEFAULT_PAGE_DIM_TOL
    );
    xml_bbox_collection_destroy(collection);
    if (!filtered_coll) {
        goto CollectionMakeFailed;
    }

    xml_bbox_collection_sort_dim(filtered_coll);

    unsigned int item_count = xml_bbox_collection_get_count(filtered_coll);
    printf("Obtained %d XML elements\n", item_count);
    for (unsigned int i=0; i < item_count; i++) {
        XmlBboxItem const * item = xml_bbox_collection_getc(filtered_coll, i);
        xmlNode const *node = item->node;
        Box2D bbox = item->bbox;
        printf(
            "%s with bounding box (%.2f, %.2f) %.2f x %.2f",
            (char const *) node->name,
            bbox.tl.x, bbox.tl.y,
            bbox.size.width, bbox.size.height
        );
        printf("\n");
    }

    xml_bbox_collection_destroy(filtered_coll);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;

CollectionMakeFailed:
    xmlFreeDoc(doc);
    xmlCleanupParser();
DocumentParseFailed:
InvalidArgs:
    return 1;
}


XmlBboxCollection * xml_bbox_collection_filter_by_dim(
    XmlBboxCollection const *collection,
    Size2D target_size,
    float tol
) {
    XmlBboxCollection *new_coll = xml_bbox_collection_make();
    if (!new_coll) {
        return NULL;
    }
    unsigned int const item_count = xml_bbox_collection_get_count(collection);
    for (unsigned int i = 0; i < item_count; i++) {
        XmlBboxItem const *item = xml_bbox_collection_getc(collection, i);
        Size2D item_size = item->bbox.size;

        if (
            item_size.width * (1+tol) < target_size.width
            || target_size.width * (1+tol) < item_size.width
        ) {
            continue;
        }

        if (
            item_size.height * (1+tol) < target_size.height
            || target_size.height * (1+tol) < item_size.height
        ) {
            continue;
        }

        xml_bbox_collection_append(new_coll, item->node, item->bbox);
    }

    return new_coll;
};

