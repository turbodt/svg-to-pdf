#include "./pdf-writer.h"
#include <stdio.h>
#include <stdlib.h>


int pdf_write_file(
    char const *filename,
    PdfBuf const *content,
    double width,
    double height
) {
    PdfPage page = {
        .data = content->data,
        .len = content->len,
        .width = width,
        .height = height,
    };
    return pdf_write_pages(filename, &page, 1);
}


int pdf_write_pages(
    char const *filename,
    PdfPage const *pages,
    unsigned int page_count
) {
    void *data = NULL;
    unsigned long len = 0;
    if (pdf_write_pages_to_memory(pages, page_count, &data, &len)) {
        return 1;
    }

    FILE *f = fopen(filename, "wb");
    if (!f) {
        free(data);
        return 1;
    }
    int err = fwrite(data, 1, len, f) != len;
    free(data);
    fclose(f);
    return err;
}


int pdf_write_pages_to_memory(
    PdfPage const *pages,
    unsigned int page_count,
    void **out_data,
    unsigned long *out_len
) {
    if (!pages || page_count == 0 || !out_data || !out_len) {
        return 1;
    }

    *out_data = NULL;
    *out_len = 0;

    unsigned int const font_id = 3 + 2 * page_count;
    unsigned int const object_count = font_id;
    long *offsets = calloc(object_count + 1, sizeof(long));
    if (!offsets) {
        return 1;
    }

    PdfBuf out = {0};
    pdf_buf_printf(&out, "%%PDF-1.4\n%%\xE2\xE3\xCF\xD3\n");
    offsets[1] = (long) out.len;
    pdf_buf_printf(&out, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");
    offsets[2] = (long) out.len;

    pdf_buf_printf(&out, "2 0 obj\n<< /Type /Pages /Kids [");
    for (unsigned int i = 0; i < page_count; i++) {
        pdf_buf_printf(&out, "%u 0 R ", 3 + i);
    }
    pdf_buf_printf(&out, "] /Count %u >>\nendobj\n", page_count);

    for (unsigned int i = 0; i < page_count; i++) {
        unsigned int page_id = 3 + i;
        unsigned int content_id = 3 + page_count + i;
        offsets[page_id] = (long) out.len;
        pdf_buf_printf(
            &out,
            "%u 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 %.6f %.6f] "
            "/Resources << /Font << /F1 %u 0 R >> >> /Contents %u 0 R >>\nendobj\n",
            page_id,
            pages[i].width,
            pages[i].height,
            font_id,
            content_id
        );
    }

    for (unsigned int i = 0; i < page_count; i++) {
        unsigned int content_id = 3 + page_count + i;
        offsets[content_id] = (long) out.len;
        pdf_buf_printf(&out, "%u 0 obj\n<< /Length %lu >>\nstream\n", content_id, pages[i].len);
        pdf_buf_write(&out, pages[i].data, pages[i].len);
        pdf_buf_printf(&out, "\nendstream\nendobj\n");
    }

    offsets[font_id] = (long) out.len;
    pdf_buf_printf(&out, "%u 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n", font_id);
    long xref = (long) out.len;
    pdf_buf_printf(&out, "xref\n0 %u\n0000000000 65535 f \n", object_count + 1);
    for (unsigned int i = 1; i <= object_count; i++) {
        pdf_buf_printf(&out, "%010ld 00000 n \n", offsets[i]);
    }
    pdf_buf_printf(&out, "trailer\n<< /Size %u /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", object_count + 1, xref);

    free(offsets);
    *out_data = out.data;
    *out_len = (unsigned long) out.len;
    return 0;
}
