#ifndef APP_PDF_WRITER_H
#define APP_PDF_WRITER_H


#include "./pdf-buffer.h"


int pdf_write_file(
    char const *filename,
    PdfBuf const *content,
    double width,
    double height
);


#endif
