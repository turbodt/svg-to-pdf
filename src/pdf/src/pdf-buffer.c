#include "./pdf-buffer.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


int pdf_buf_printf(PdfBuf *buf, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);
    if (needed < 0) {
        va_end(args);
        return 1;
    }
    size_t target = buf->len + (size_t)needed + 1;
    if (target > buf->cap) {
        size_t cap = buf->cap ? buf->cap : 4096;
        while (cap < target) {
            cap *= 2;
        }
        char *data = realloc(buf->data, cap);
        if (!data) {
            va_end(args);
            return 1;
        }
        buf->data = data;
        buf->cap = cap;
    }
    vsnprintf(buf->data + buf->len, buf->cap - buf->len, fmt, args);
    buf->len += (size_t)needed;
    va_end(args);
    return 0;
}


void pdf_buf_free(PdfBuf *buf) {
    free(buf->data);
    *buf = (PdfBuf){0};
}
