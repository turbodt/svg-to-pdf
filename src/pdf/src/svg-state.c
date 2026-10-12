#include "./shared.h"
#include <ctype.h>
#include <string.h>


RenderState render_state_child(xmlNode *node, RenderState const *parent) {
    RenderState state = *parent;
    state.transform = geo_transform_copy(parent->transform);
    char *transform = attr_string(node, "transform");
    if (transform) {
        svg_transform_perform_operation(state.transform, transform);
        xmlFree(transform);
    }

    for (xmlAttr *attr = node->properties; attr; attr = attr->next) {
        xmlChar *value = xmlNodeListGetString(node->doc, attr->children, 1);
        if (value) {
            apply_style(&state, (char const *)attr->name, (char const *)value);
            xmlFree(value);
        }
    }
    char *style = attr_string(node, "style");
    if (style) {
        char *p = style;
        while (*p) {
            while (*p == ';' || isspace((unsigned char)*p)) p++;
            char *name = p;
            while (*p && *p != ':' && *p != ';') p++;
            if (*p != ':') break;
            *p++ = '\0';
            char *value = p;
            while (*p && *p != ';') p++;
            if (*p) *p++ = '\0';
            apply_style(&state, name, value);
        }
        xmlFree(style);
    }
    return state;
}


void render_state_destroy(RenderState *state) {
    geo_transform_destroy(state->transform);
    state->transform = NULL;
}
