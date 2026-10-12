#include <pdf-renderer.h>
#include "./pdf-writer.h"
#include "./shared.h"
#include <stdio.h>
#include <stdlib.h>


static void render_node(
    xmlNode *node,
    PdfBuf *out,
    RenderState const *state,
    xmlDoc *doc
);


int pdf_render_svg_doc(xmlDoc *doc, char const *filename) {
    return pdf_render_svg_doc_with_options(doc, filename, NULL);
}


int pdf_render_svg_doc_with_options(
    xmlDoc *doc,
    char const *filename,
    PdfRenderOptions const *options
) {
    PdfPage page = {0};
    int err = pdf_render_svg_doc_to_page(doc, options, &page);
    if (err) {
        return err;
    }
    err = pdf_write_pages(filename, &page, 1);
    pdf_page_destroy(&page);
    return err;
}


int pdf_render_svg_doc_to_page(
    xmlDoc *doc,
    PdfRenderOptions const *options,
    PdfPage *out
) {
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
    state.page_width = options && options->has_page_size
        ? options->page_size.width
        : state.viewbox.size.width;
    state.page_height = options && options->has_page_size
        ? options->page_size.height
        : state.viewbox.size.height;
    if (state.page_width <= 0 || state.page_height <= 0) {
        geo_transform_destroy(state.transform);
        return 1;
    }
    double scale_x = state.page_width / state.viewbox.size.width;
    double scale_y = state.page_height / state.viewbox.size.height;
    state.scale = scale_x < scale_y ? scale_x : scale_y;
    state.offset_x = (state.page_width - state.viewbox.size.width * state.scale) / 2;
    state.offset_y = (state.page_height - state.viewbox.size.height * state.scale) / 2;
    PdfBuf content = {0};
    pdf_buf_printf(
        &content,
        "q\n%.6f %.6f %.6f %.6f re W n\n",
        state.offset_x,
        state.offset_y,
        state.viewbox.size.width * state.scale,
        state.viewbox.size.height * state.scale
    );
    render_node(root->children, &content, &state, doc);
    pdf_buf_printf(&content, "Q\n");

    *out = (PdfPage){
        .data = content.data,
        .len = content.len,
        .width = state.page_width,
        .height = state.page_height,
    };
    geo_transform_destroy(state.transform);
    return 0;
}


void pdf_page_destroy(PdfPage *page) {
    free(page->data);
    *page = (PdfPage){0};
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
