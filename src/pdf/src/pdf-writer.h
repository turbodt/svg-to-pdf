#ifndef APP_PDF_WRITER_H
#define APP_PDF_WRITER_H


#include "./pdf-buffer.h"
#include <pdf-renderer.h>


int pdf_write_file(
    char const *filename,
    PdfBuf const *content,
    double width,
    double height
);
int pdf_write_pages(
    char const *filename,
    PdfPage const *pages,
    unsigned int page_count
);


#endif
