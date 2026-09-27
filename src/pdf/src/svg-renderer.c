#include <pdf-renderer.h>
#include "./pdf-writer.h"
#include "./shared.h"
#include <stdio.h>


static void render_node(
    xmlNode *node,
    PdfBuf *out,
    RenderState const *state,
    xmlDoc *doc
);


int pdf_render_svg_doc(xmlDoc *doc, char const *filename) {
    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root) {
        return 1;
    }

    RenderState state = {0};
    state.transform = geo_transform_make();
    if (!state.transform) {
        return 1;
    }
    state.fill = (PdfColor){.enabled=1, .r=0, .g=0, .b=0};
    state.stroke = (PdfColor){.enabled=0};
    state.stroke_width = 1;
    state.linecap = 0;
    state.viewbox.tl.x = 0;
    state.viewbox.tl.y = 0;
    state.viewbox.size.width = attr_double(root, "width", 0);
    state.viewbox.size.height = attr_double(root, "height", 0);

    char *viewbox = attr_string(root, "viewBox");
    if (viewbox) {
        sscanf(
            viewbox,
            "%lf %lf %lf %lf",
            &state.viewbox.tl.x,
            &state.viewbox.tl.y,
            &state.viewbox.size.width,
            &state.viewbox.size.height
        );
        xmlFree(viewbox);
    }
    state.page_width = state.viewbox.size.width;
    state.page_height = state.viewbox.size.height;
    if (state.page_width <= 0 || state.page_height <= 0) {
        geo_transform_destroy(state.transform);
        return 1;
    }

    PdfBuf content = {0};
    pdf_buf_printf(
        &content,
        "q\n0 0 %.6f %.6f re W n\n",
        state.page_width,
        state.page_height
    );
    render_node(root->children, &content, &state, doc);
    pdf_buf_printf(&content, "Q\n");

    int err = pdf_write_file(
        filename,
        &content,
        state.page_width,
        state.page_height
    );
    pdf_buf_free(&content);
    geo_transform_destroy(state.transform);
    return err;
}


static void render_node(
    xmlNode *node,
    PdfBuf *out,
    RenderState const *state,
    xmlDoc *doc
) {
    for (xmlNode *cur = node; cur; cur = cur->next) {
        if (cur->type != XML_ELEMENT_NODE) {
            continue;
        }
        if (xmlStrEqual(cur->name, (xmlChar const *)"defs")
            || xmlStrEqual(cur->name, (xmlChar const *)"metadata")
            || xmlStrEqual(cur->name, (xmlChar const *)"style")
            || xmlStrEqual(cur->name, (xmlChar const *)"mask")) {
            continue;
        }

        RenderState child = render_state_child(cur, state);
        int clipped = emit_clip(cur, out, &child, doc);
        if (xmlStrEqual(cur->name, (xmlChar const *)"path")) {
            render_path(cur, out, &child);
        } else if (xmlStrEqual(cur->name, (xmlChar const *)"rect")) {
            render_rect(cur, out, &child);
        } else if (xmlStrEqual(cur->name, (xmlChar const *)"text")) {
            render_text(cur, out, &child);
        }
        render_node(cur->children, out, &child, doc);
        if (clipped) {
            pdf_buf_printf(out, "Q\n");
        }
        render_state_destroy(&child);
    }
}
