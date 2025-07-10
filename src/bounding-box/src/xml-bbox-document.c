#include "./shared.h"
#include <libxml/tree.h>


static int need_clean_up = 0;


void bbox_clean_up(void) {
    if (!need_clean_up) {
        return;
    }
    xmlCleanupParser();
    need_clean_up = 0;
};


inline SvgDocument * bbox_svg_doc_make_from_file(char const *filename) {
    SvgDocument *impl = (SvgDocument *) xmlReadFile(filename, NULL, 0);
    if (impl) {
        need_clean_up = 1;
    }
    return impl;
};


inline void bbox_svg_doc_destroy(SvgDocument *doc) {
    xmlFreeDoc(&doc->xmlDoc);
};


BboxCollection * bbox_collection_make_from_doc(SvgDocument *doc) {
    xmlNode *root = xmlDocGetRootElement(&doc->xmlDoc);
    BboxCollection *collection = bbox_collection_make();
    if (!collection) {
        goto CollectionMakeFromDocCollMakeFailed;
    }
    xml_bbox_collection_make_from_node(root, collection, NULL);
    return collection;
CollectionMakeFromDocCollMakeFailed:
    return NULL;
};


inline void bbox_svg_doc_save_file(SvgDocument *doc, char const *filename) {
    xmlSaveFormatFileEnc(filename, &doc->xmlDoc, "UTF-8", 1);
};


void bbox_svg_doc_set_viewbox(SvgDocument *doc, Box2D viewbox) {
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

    xmlNode *root = xmlDocGetRootElement(&doc->xmlDoc);
    xmlSetProp(root, (const xmlChar *)"viewBox", (const xmlChar *)viewbox_str);
    xmlSetProp(root, (const xmlChar *)"width", (const xmlChar *)width_str);
    xmlSetProp(root, (const xmlChar *)"height", (const xmlChar *)height_str);
};
