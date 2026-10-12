#ifndef APP_PDF_BUFFER_H
#define APP_PDF_BUFFER_H


#include <stddef.h>


typedef struct {
    char *data;
    size_t len;
    size_t cap;
} PdfBuf;


int pdf_buf_printf(PdfBuf *buf, char const *fmt, ...);
int pdf_buf_write(PdfBuf *buf, void const *data, size_t len);
void pdf_buf_free(PdfBuf *buf);


#endif
