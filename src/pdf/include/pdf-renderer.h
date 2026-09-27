#ifndef APP_PDF_RENDERER_H
#define APP_PDF_RENDERER_H


#include <libxml/tree.h>


int pdf_render_svg_doc(xmlDoc *doc, char const *filename);


#endif
