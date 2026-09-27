#include "./shared.h"
#include <string.h>


static void pdf_escape_text(PdfBuf *out, char const *s);


void render_path(xmlNode *node, PdfBuf *out, RenderState const *state) {
    char *d = attr_string(node, "d");
    if (!d) {
        return;
    }
    SVGPath *path = svg_path_make_from_string(d);
    xmlFree(d);
    if (!path) {
        return;
    }
    svg_path_apply_transform(path, state->transform);
    unsigned int count = svg_path_command_count(path);
    int has_path = 0;
    Point2D last = {0};
    for (unsigned int i = 0; i < count; i++) {
        SVGPathCommand const *cmd = svg_path_get_command(path, i);
        if (!has_path || cmd->start.x != last.x || cmd->start.y != last.y) {
            emit_point(out, state, cmd->start);
            pdf_buf_printf(out, " m\n");
        }
        if (cmd->type == SVG_PATH_CMD_LINE) {
            emit_point(out, state, cmd->end);
            pdf_buf_printf(out, " l\n");
        } else if (cmd->type == SVG_PATH_CMD_QBEZIER) {
            Point2D c1 = {
                cmd->start.x + 2.0/3.0 * (cmd->ctrl[0].x - cmd->start.x),
                cmd->start.y + 2.0/3.0 * (cmd->ctrl[0].y - cmd->start.y)
            };
            Point2D c2 = {
                cmd->end.x + 2.0/3.0 * (cmd->ctrl[0].x - cmd->end.x),
                cmd->end.y + 2.0/3.0 * (cmd->ctrl[0].y - cmd->end.y)
            };
            emit_point(out, state, c1); pdf_buf_printf(out, " ");
            emit_point(out, state, c2); pdf_buf_printf(out, " ");
            emit_point(out, state, cmd->end); pdf_buf_printf(out, " c\n");
        } else if (cmd->type == SVG_PATH_CMD_CBEZIER) {
            emit_point(out, state, cmd->ctrl[0]); pdf_buf_printf(out, " ");
            emit_point(out, state, cmd->ctrl[1]); pdf_buf_printf(out, " ");
            emit_point(out, state, cmd->end); pdf_buf_printf(out, " c\n");
        }
        last = cmd->end;
        has_path = 1;
    }
    emit_paint(out, state);
    svg_path_destroy(path);
}


void render_rect(xmlNode *node, PdfBuf *out, RenderState const *state) {
    double x = attr_double(node, "x", 0);
    double y = attr_double(node, "y", 0);
    double w = attr_double(node, "width", 0);
    double h = attr_double(node, "height", 0);
    if (w <= 0 || h <= 0) {
        return;
    }
    Point2D p[4] = {{x,y}, {x+w,y}, {x+w,y+h}, {x,y+h}};
    for (int i = 0; i < 4; i++) {
        p[i] = geo_transform_apply_point(state->transform, p[i]);
    }
    emit_point(out, state, p[0]); pdf_buf_printf(out, " m\n");
    for (int i = 1; i < 4; i++) {
        emit_point(out, state, p[i]); pdf_buf_printf(out, " l\n");
    }
    pdf_buf_printf(out, "h\n");
    emit_paint(out, state);
}


void render_text(xmlNode *node, PdfBuf *out, RenderState const *state) {
    xmlChar *text = xmlNodeGetContent(node);
    if (!text || !*text || !state->fill.enabled) {
        if (text) xmlFree(text);
        return;
    }
    double x = attr_double(node, "x", 0);
    double y = attr_double(node, "y", 0);
    double font_size = attr_double(node, "font-size", 12);
    Point2D p = geo_transform_apply_point(state->transform, (Point2D){x, y});
    p = map_point(state, p);
    pdf_buf_printf(
        out,
        "q\n%.6f %.6f %.6f rg\nBT /F1 %.6f Tf 1 0 0 1 %.6f %.6f Tm (",
        state->fill.r,
        state->fill.g,
        state->fill.b,
        font_size,
        p.x,
        p.y
    );
    pdf_escape_text(out, (char const *)text);
    pdf_buf_printf(out, ") Tj ET\nQ\n");
    xmlFree(text);
}


void emit_paint(PdfBuf *out, RenderState const *state) {
    if (!state->fill.enabled && !state->stroke.enabled) {
        pdf_buf_printf(out, "n\n");
        return;
    }
    if (state->fill.enabled) {
        pdf_buf_printf(out, "%.6f %.6f %.6f rg\n", state->fill.r, state->fill.g, state->fill.b);
    }
    if (state->stroke.enabled) {
        pdf_buf_printf(
            out,
            "%.6f %.6f %.6f RG\n%.6f w\n%d J\n",
            state->stroke.r,
            state->stroke.g,
            state->stroke.b,
            state->stroke_width,
            state->linecap
        );
        if (state->dash[0]) {
            pdf_buf_printf(out, "[%s] 0 d\n", state->dash);
        } else {
            pdf_buf_printf(out, "[] 0 d\n");
        }
    }
    if (state->fill.enabled && state->stroke.enabled) {
        pdf_buf_printf(out, "B\n");
    } else if (state->fill.enabled) {
        pdf_buf_printf(out, "f\n");
    } else {
        pdf_buf_printf(out, "S\n");
    }
}


void emit_point(PdfBuf *out, RenderState const *state, Point2D p) {
    p = map_point(state, p);
    pdf_buf_printf(out, "%.6f %.6f", p.x, p.y);
}


Point2D map_point(RenderState const *state, Point2D p) {
    return (Point2D){
        .x = p.x - state->viewbox.tl.x,
        .y = state->viewbox.tl.y + state->viewbox.size.height - p.y,
    };
}


static void pdf_escape_text(PdfBuf *out, char const *s) {
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '(' || c == ')' || c == '\\') {
            pdf_buf_printf(out, "\\%c", c);
        } else if (c >= 32 && c < 127) {
            pdf_buf_printf(out, "%c", c);
        } else if (c == '\n' || c == '\r' || c == '\t') {
            pdf_buf_printf(out, " ");
        } else if ((c & 0x80) == 0) {
            pdf_buf_printf(out, "?");
        }
    }
}
