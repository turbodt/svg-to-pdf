#include "./shared.h"
#include <stdlib.h>
#include <libxml/tree.h>


static int trim_recursive(
    xmlNode const *o_node,
    xmlNode *node,
    BboxCollection const *to_keep_collection
);


SvgDocument * bbox_svg_doc_make_from_subset(
    SvgDocument *doc,
    BboxCollection const *to_keep
) {
    xmlDoc *new_doc = xmlCopyDoc(&doc->xmlDoc, 1);
    if (!new_doc) {
        return NULL;
    }

    xmlNode *o_root = xmlDocGetRootElement(&doc->xmlDoc);
    xmlNode *root = xmlDocGetRootElement(new_doc);
    trim_recursive(o_root, root, to_keep);
    return (SvgDocument *) new_doc;
};


int trim_recursive(
    xmlNode const *o_node,
    xmlNode *node,
    BboxCollection const *to_keep_coll
) {
    int is_keep = 0;
    unsigned int to_keep_count = bbox_collection_get_count(to_keep_coll);

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
                BboxItem const *to_keep_item;
                to_keep_item = bbox_collection_getc(to_keep_coll, i);
                if (to_keep_item->id == o_node_cur) {
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
