#include "./shared.h"
#include <custom-svg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <math.h>


static xmlChar const *tag_transform = (xmlChar const *)"transform";
static xmlChar const *tag_path = (xmlChar const *)"path";
static xmlChar const *tag_d = (xmlChar const *)"d";
static xmlChar const *tag_rect = (xmlChar const *)"rect";
static xmlChar const *tag_width = (xmlChar const *)"width";
static xmlChar const *tag_height = (xmlChar const *)"height";
static xmlChar const *tag_x = (xmlChar const *)"x";
static xmlChar const *tag_y = (xmlChar const *)"y";
static xmlChar const *tag_defs = (xmlChar const *)"defs";

static Box2D xml_path_extract_bbox(xmlNode *node, Transform const *trans);
static Box2D xml_rect_extract_bbox(xmlNode *node, Transform const *trans);
static Transform * parse_transform(Transform const *curr, const char *str);
static double min4(double, double, double, double);
static double max4(double, double, double, double);


void xml_bbox_collection_make_from_node(
    xmlNode *node,
    BboxCollection *collection,
    Transform * trans_acc
) {
    if (!trans_acc) {
        trans_acc = geo_transform_make();
        xml_bbox_collection_make_from_node(node, collection, trans_acc);
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

        Box2D bbox = {0};
        if (xmlStrEqual(node_cur->name, tag_path)) {
            bbox = xml_path_extract_bbox(node_cur, trans_cur);
        } else if (xmlStrEqual(node_cur->name, tag_rect)) {
            bbox = xml_rect_extract_bbox(node_cur, trans_cur);
        }

        if (bbox.size.width != 0 && bbox.size.height != 0) {
            bbox_collection_append(collection, node_cur, bbox);
        }

        xml_bbox_collection_make_from_node(
            node_cur->children,
            collection,
            trans_cur
        );

        if (transform_attr) {
            geo_transform_destroy(trans_cur);
            trans_cur = NULL;
        }
    }
}


Box2D xml_path_extract_bbox(xmlNode *node, Transform const *trans) {
    xmlChar *d = xmlGetProp(node, tag_d);
    if (!d) {
        return (Box2D){0};
    }

    SVGPath *path = svg_path_make_from_string((char const *)d);
    xmlFree(d);
    if (!path) {
        return (Box2D){0};
    }
    svg_path_apply_transform(path, trans);

    Box2D bbox = svg_path_get_bbox(path);

    svg_path_destroy(path);

    return bbox;
}


Box2D xml_rect_extract_bbox(xmlNode *node, Transform const *trans) {
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
