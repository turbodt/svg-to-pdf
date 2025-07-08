#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <math.h>
#include <custom-svg.h>
#include "./xml-bbox-collection.h"


static xmlChar const *tag_transform = (xmlChar const *)"transform";
static xmlChar const *tag_path = (xmlChar const *)"path";
static xmlChar const *tag_d = (xmlChar const *)"d";
static xmlChar const *tag_rect = (xmlChar const *)"rect";
static xmlChar const *tag_width = (xmlChar const *)"width";
static xmlChar const *tag_height = (xmlChar const *)"height";
static xmlChar const *tag_x = (xmlChar const *)"x";
static xmlChar const *tag_y = (xmlChar const *)"y";
static xmlChar const *tag_defs = (xmlChar const *)"defs";
static double min4(double, double, double, double);
static double max4(double, double, double, double);


Transform * parse_transform(Transform const *curr, const char *str) {
    Transform *t = geo_transform_copy(curr);
    if (!t) {
        return NULL;
    }
    if (!str) {
        return t;
    }

    svg_transform_perform_operation(t, str);

    return t;
}


static Box2D xml_rect_extract_bbox(xmlNode *node, Transform const *trans) {
    Size2D size;
    Point2D tl = {0};
    if (!node || !xmlStrEqual(node->name, tag_rect)) {
        return (Box2D){0};
    }
    char const *p;

    char *width_str = (char *) xmlGetProp(node, tag_width);
    if (!width_str) {
        return (Box2D){0};
    }
    p = svg_double_parse(width_str, &size.width);
    xmlFree(width_str);
    if (p == width_str) {
        return (Box2D){0};
    }

    char *height_str = (char *) xmlGetProp(node, tag_height);
    if (!height_str) {
        return (Box2D){0};
    }
    p = svg_double_parse(height_str, &size.height);
    xmlFree(height_str);
    if (p == height_str) {
        return (Box2D){0};
    }

    char *x_str = (char *) xmlGetProp(node, tag_x);
    char *y_str = (char *) xmlGetProp(node, tag_y);
    if (x_str) {
        svg_double_parse(x_str, &tl.x);
        xmlFree(x_str);
    }
    if (y_str) {
        svg_double_parse(y_str, &tl.y);
        xmlFree(y_str);
    }

    Point2D tr = tl, bl = tl, br = tl;
    tr.x += size.width;
    bl.y += size.height;
    br.x += size.width;
    br.y += size.height;
    tl = geo_transform_apply_point(trans, tl);
    tr = geo_transform_apply_point(trans, tr);
    bl = geo_transform_apply_point(trans, bl);
    br = geo_transform_apply_point(trans, br);

    double x_min = min4(tl.x, tr.x, bl.x, br.x);
    double x_max = max4(tl.x, tr.x, bl.x, br.x);
    double y_min = min4(tl.y, tr.y, bl.y, br.y);
    double y_max = max4(tl.y, tr.y, bl.y, br.y);

    return (Box2D) {
        .tl = {.x=x_min, .y=y_min},
        .size = {.width=x_max - x_min, .height=y_max - y_min},
    };
};


void get_xml_bbox_collection(
    xmlNode *node,
    XmlBboxCollection *collection,
    Transform * trans_acc
) {
    if (!trans_acc) {
        trans_acc = geo_transform_make();
        get_xml_bbox_collection(node, collection, trans_acc);
        geo_transform_destroy(trans_acc);
        return;
    }

    for (xmlNode *node_cur = node; node_cur; node_cur = node_cur->next) {
        if (node_cur->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(node_cur->name, tag_defs)) {
            continue;
        }

        Transform *trans_cur = trans_acc;
        xmlChar *transform_attr = xmlGetProp(node_cur, tag_transform);

        if (transform_attr) {
            trans_cur = parse_transform(
                trans_cur,
                (const char *)transform_attr
            );
            xmlFree(transform_attr);
        }

        if (xmlStrEqual(node_cur->name, tag_path)) {
            xmlChar *d = xmlGetProp(node_cur, tag_d);
            if (d) {
                SVGPath *path = svg_path_make_from_string((char const *)d);
                svg_path_apply_transform(path, trans_cur);

                Box2D bbox = svg_path_get_bbox(path);
                xml_bbox_collection_append(collection, node_cur, bbox);

                svg_path_destroy(path);
                xmlFree(d);
            }
        } else if (xmlStrEqual(node_cur->name, tag_rect)) {
            Box2D bbox = xml_rect_extract_bbox(node_cur, trans_cur);
            xml_bbox_collection_append(collection, node_cur, bbox);
        }

        get_xml_bbox_collection(node_cur->children, collection, trans_cur);

        if (transform_attr) {
            geo_transform_destroy(trans_cur);
            trans_cur = NULL;
        }
    }
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file.svg\n", argv[0]);
        return 1;
    }

    xmlDoc *doc = xmlReadFile(argv[1], NULL, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse %s\n", argv[1]);
        goto DocumentParseFailed;
    }

    XmlBboxCollection *collection = xml_bbox_collection_make();
    if (!collection) {
        goto CollectionMakeFailed;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    get_xml_bbox_collection(root, collection, NULL);
    xml_bbox_collection_sort_dim(collection);

    unsigned int item_count = xml_bbox_collection_get_count(collection);
    printf("Obtained %d XML elements\n", item_count);
    for (unsigned int i=0; i < item_count; i++) {
        XmlBboxItem const * item = xml_bbox_collection_getc(collection, i);
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

    xml_bbox_collection_destroy(collection);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;

CollectionMakeFailed:
    xmlFreeDoc(doc);
DocumentParseFailed:
    return 1;
}


double min4(double a, double b, double c, double d) {
    if (a > b) {
        a = b;
    }
    if (a > c) {
        a = c;
    }
    if (a > d) {
        a = d;
    }
    return a;
};


double max4(double a, double b, double c, double d) {
    if (a < b) {
        a = b;
    }
    if (a < c) {
        a = c;
    }
    if (a < d) {
        a = d;
    }
    return a;
};
