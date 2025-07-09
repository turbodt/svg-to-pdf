#include <stdio.h>
#include <errno.h>
#include "./xml-bbox-extractor.h"
#include <geometry.h>


#define DEFAULT_PAGE_WIDTH 2400
#define DEFAULT_PAGE_HEIGHT 3400
#define DEFAULT_PAGE_DIM_TOL 0.05
#define MAX_PAGE_COUNT 100


static XmlBboxCollection *xml_bbox_collection_filter_by_dim(
    XmlBboxCollection const *collection,
    Size2D target_size,
    float tolerance
);

static XmlBboxCollection *xml_bbox_collection_filter_by_item(
    XmlBboxCollection const *collection,
    XmlBboxItem const *item
);

static void xml_node_set_viewbox(xmlNode *node, Box2D);

int trim_recursive(
    xmlNode const *o_node,
    xmlNode *node,
    XmlBboxCollection const *to_keep_coll
) {
    int is_keep = 0;
    unsigned int to_keep_count = xml_bbox_collection_get_count(to_keep_coll);

    xmlNode *node_cur = node;
    xmlNode const *o_node_cur = o_node;
    while (node_cur) {
        xmlNode *node_next = node_cur->next;

        int is_node_cur_keep = 0;

        if (node_cur->type != XML_ELEMENT_NODE) {
            is_node_cur_keep = 1;
        } else if (xmlStrEqual(node_cur->name, (xmlChar const *)"defs")) {
            is_node_cur_keep = 1;
        } else {
            int was_found = 0;
            for (unsigned int i = 0; i < to_keep_count && !was_found; i++) {
                XmlBboxItem const *to_keep_item;
                to_keep_item = xml_bbox_collection_getc(to_keep_coll, i);
                if (to_keep_item->node == o_node_cur) {
                    was_found = 1;
                }
            }

            if (was_found) {
                is_node_cur_keep = 1;
            }
        }

        if (is_node_cur_keep) {
            is_keep = 1;
        }

        int children_keep = trim_recursive(
            o_node_cur->children,
            node_cur->children,
            to_keep_coll
        );

        if (children_keep) {
            is_keep = 1;
        }

        if (!is_node_cur_keep && !children_keep) {
            xmlUnlinkNode(node_cur);
        }

        node_cur = node_next;
        o_node_cur = o_node_cur->next;
    }

    return is_keep;
}


int main(int argc, char **argv) {
    XmlBboxCollection *pages[MAX_PAGE_COUNT] = {0};
    xmlDoc *page_docs[MAX_PAGE_COUNT] = {0};
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
    if (!filtered_coll) {
        goto FilteredCollectionMakeFailed;
    }

    xml_bbox_collection_sort_dim(filtered_coll);

    unsigned int page_count = xml_bbox_collection_get_count(filtered_coll);
    printf(
        "Obtained %d XML page elements out of %d.\n",
        page_count,
        xml_bbox_collection_get_count(collection)
    );
    /*
    for (unsigned int i=0; i < page_count; i++) {
        XmlBboxItem const * item = xml_bbox_collection_getc(filtered_coll, i);
        xmlNode const *node = item->node;
        Box2D bbox = item->bbox;
        printf(
            "inkscape %s"
            " --export-area-drawing=%.0f:%.0f:%.0f:%.0f"
            " --export-filename=./data/page-%d.pdf",
            argv[1],
            bbox.tl.x, bbox.tl.y,
            bbox.tl.x + bbox.size.width, bbox.tl.y + bbox.size.height,
            i + 1
        );
        printf("\n");
    }
    */

    for (unsigned int i=0; i < page_count; i++) {
        XmlBboxItem const * item = xml_bbox_collection_getc(filtered_coll, i);
        pages[i] = xml_bbox_collection_filter_by_item(collection, item);
        if (!pages[i]) {
            continue;
        }

        printf(
            "For page %d we have %d elements.\n",
            i+1,
            xml_bbox_collection_get_count(pages[i])
        );

        page_docs[i] = xmlCopyDoc(doc, 1);
        xmlNode *o_root = xmlDocGetRootElement(doc);
        xmlNode *root = xmlDocGetRootElement(page_docs[i]);
        trim_recursive(o_root, root, pages[i]);
        xml_node_set_viewbox(root, item->bbox);
    }

    for (unsigned int i=0; i < page_count; i++) {
        char filename[32];
        snprintf(filename, 32, "./data/page-%d.svg", i);
        xmlSaveFormatFileEnc(filename, page_docs[i], "UTF-8", 1);
        xmlFreeDoc(page_docs[i]);
        xml_bbox_collection_destroy(pages[i]);
    }

    xml_bbox_collection_destroy(filtered_coll);
    xml_bbox_collection_destroy(collection);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;

FilteredCollectionMakeFailed:
    xml_bbox_collection_destroy(collection);
CollectionMakeFailed:
    xmlFreeDoc(doc);
    xmlCleanupParser();
DocumentParseFailed:
InvalidArgs:
    return 1;
}


void xml_node_set_viewbox(xmlNode *node, Box2D viewbox) {
    char viewbox_str[256], width_str[64], height_str[64];
    snprintf(
        viewbox_str,
        sizeof(viewbox_str),
        "%f %f %f %f",
        viewbox.tl.x,
        viewbox.tl.y,
        viewbox.size.width,
        viewbox.size.height
    );
    snprintf(width_str, sizeof(width_str), "%f", viewbox.size.width);
    snprintf(height_str, sizeof(height_str), "%f", viewbox.size.height);

    xmlSetProp(node, (const xmlChar *)"viewBox", (const xmlChar *)viewbox_str);
    xmlSetProp(node, (const xmlChar *)"width", (const xmlChar *)width_str);
    xmlSetProp(node, (const xmlChar *)"height", (const xmlChar *)height_str);
};


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


XmlBboxCollection *xml_bbox_collection_filter_by_item(
    XmlBboxCollection const *collection,
    XmlBboxItem const *container_item
) {
    XmlBboxCollection *new_coll = xml_bbox_collection_make();
    if (!new_coll) {
        return NULL;
    }
    unsigned int const item_count = xml_bbox_collection_get_count(collection);
    for (unsigned int i = 0; i < item_count; i++) {
        XmlBboxItem const *item = xml_bbox_collection_getc(collection, i);
        if (container_item->node == item->node) {
            continue;
        }
        if (!geo_boxes_intersect(container_item->bbox, item->bbox)) {
            continue;
        }
        xml_bbox_collection_append(new_coll, item->node, item->bbox);
    }

    return new_coll;
};
