// svg_parser.c
// Extract all <path> elements and apply inherited group transformations using libxml2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <math.h>
#include "./svg/main.h"


Transform * parse_transform(Transform const *curr, const char *str) {
    Transform *t = transform_copy(curr);
    if (!t) {
        return NULL;
    }
    if (!str) {
        return t;
    }

    svg_transform_perform_operation(t, str);

    return t;
}

void extract_paths(xmlNode *node, Transform * accumulated) {
    static xmlChar const *tag_transform = (xmlChar const *)"transform";
    static xmlChar const *tag_path = (xmlChar const *)"path";
    static xmlChar const *tag_d = (xmlChar const *)"d";

    if (!accumulated) {
        accumulated = transform_make();
        extract_paths(node, accumulated);
        transform_destroy(accumulated);
        return;
    }

    for (xmlNode *cur = node; cur; cur = cur->next) {
        if (cur->type != XML_ELEMENT_NODE) {
            continue;
        }

        Transform *current = accumulated;
        xmlChar *transform_attr = xmlGetProp(cur, tag_transform);

        if (transform_attr) {
            current = parse_transform(
                current,
                (const char *)transform_attr
            );
            xmlFree(transform_attr);
        }

        if (xmlStrEqual(cur->name, tag_path)) {
            xmlChar *d = xmlGetProp(cur, tag_d);
            if (d) {
                double const *m = transform_getc_matrix(current);
                printf("Path: %s\n", d);
                printf(
                    "Transformed by:\n"
                    "/ %f %f %f \\\n"
                    "| %f %f %f |\n"
                    "\\ 0 0 1 /\n",
                    m[0], m[1], m[2],
                    m[3], m[4], m[5]
                );
                xmlFree(d);
            }
        }

        extract_paths(cur->children, current);

        if (transform_attr) {
            transform_destroy(current);
            current = NULL;
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
        return 1;
    }

    xmlNode *root = xmlDocGetRootElement(doc);
    extract_paths(root, NULL);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}
