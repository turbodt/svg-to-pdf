#include "./shared.h"
#include <string.h>


static int emit_rect_clip(xmlNode *rect, PdfBuf *out, RenderState const *state);
static xmlNode *find_clip_rect(xmlDoc *doc, char const *id);


int emit_clip(xmlNode *node, PdfBuf *out, RenderState const *state, xmlDoc *doc) {
    char *clip = attr_string(node, "clip-path");
    if (!clip) {
        return 0;
    }
    char id[128];
    int has_id = parse_url_id(clip, id, sizeof(id));
    xmlFree(clip);
    if (!has_id) {
        return 0;
    }
    xmlNode *rect = find_clip_rect(doc, id);
    if (!rect) {
        return 0;
    }
    pdf_buf_printf(out, "q\n");
    if (emit_rect_clip(rect, out, state)) {
        pdf_buf_printf(out, "Q\n");
        return 0;
    }
    pdf_buf_printf(out, "W n\n");
    return 1;
}


static int emit_rect_clip(xmlNode *rect, PdfBuf *out, RenderState const *state) {
    RenderState clip_state = render_state_child(rect, state);
    double x = attr_double(rect, "x", 0);
    double y = attr_double(rect, "y", 0);
    double w = attr_double(rect, "width", 0);
    double h = attr_double(rect, "height", 0);
    if (w <= 0 || h <= 0) {
        render_state_destroy(&clip_state);
        return 1;
    }
    Point2D p[4] = {{x,y}, {x+w,y}, {x+w,y+h}, {x,y+h}};
    for (int i = 0; i < 4; i++) {
        p[i] = geo_transform_apply_point(clip_state.transform, p[i]);
    }
    emit_point(out, &clip_state, p[0]); pdf_buf_printf(out, " m\n");
    for (int i = 1; i < 4; i++) {
        emit_point(out, &clip_state, p[i]); pdf_buf_printf(out, " l\n");
    }
    pdf_buf_printf(out, "h\n");
    render_state_destroy(&clip_state);
    return 0;
}


static xmlNode *find_clip_rect(xmlDoc *doc, char const *id) {
    xmlNode *root = xmlDocGetRootElement(doc);
    xmlNode *stack[256];
    int top = 0;
    stack[top++] = root;
    while (top > 0) {
        xmlNode *node = stack[--top];
        for (xmlNode *cur = node; cur; cur = cur->next) {
            if (cur->type == XML_ELEMENT_NODE && xmlStrEqual(cur->name, (xmlChar const *)"clipPath")) {
                char *cur_id = attr_string(cur, "id");
                int matches = cur_id && strcmp(cur_id, id) == 0;
                if (cur_id) xmlFree(cur_id);
                if (matches) {
                    for (xmlNode *child = cur->children; child; child = child->next) {
                        if (child->type == XML_ELEMENT_NODE && xmlStrEqual(child->name, (xmlChar const *)"rect")) {
                            return child;
                        }
                    }
                }
            }
            if (cur->children && top < 256) {
                stack[top++] = cur->children;
            }
        }
    }
    return NULL;
}
