#ifndef APP_PDF_RENDERER_H
#define APP_PDF_RENDERER_H


#include <libxml/tree.h>
#include <geometry.h>


typedef struct {
    int has_page_size;
    Size2D page_size;
} PdfRenderOptions;


int pdf_render_svg_doc(xmlDoc *doc, char const *filename);
int pdf_render_svg_doc_with_options(
    xmlDoc *doc,
    char const *filename,
    PdfRenderOptions const *options
);


#endif
