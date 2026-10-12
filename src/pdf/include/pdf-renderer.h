#ifndef APP_PDF_RENDERER_H
#define APP_PDF_RENDERER_H


#include <libxml/tree.h>
#include <geometry.h>


typedef struct {
    int has_page_size;
    Size2D page_size;
} PdfRenderOptions;


typedef struct {
    void *data;
    unsigned long len;
    double width;
    double height;
} PdfPage;


int pdf_render_svg_doc(xmlDoc *doc, char const *filename);
int pdf_render_svg_doc_with_options(
    xmlDoc *doc,
    char const *filename,
    PdfRenderOptions const *options
);
int pdf_render_svg_doc_to_page(
    xmlDoc *doc,
    PdfRenderOptions const *options,
    PdfPage *out
);
void pdf_page_destroy(PdfPage *page);
int pdf_write_pages(
    char const *filename,
    PdfPage const *pages,
    unsigned int page_count
);
int pdf_write_pages_to_memory(
    PdfPage const *pages,
    unsigned int page_count,
    void **out_data,
    unsigned long *out_len
);


#endif
