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
    if (!pages || page_count == 0) {
        return 1;
    }

    FILE *f = fopen(filename, "wb");
    if (!f) {
        return 1;
    }

    unsigned int const font_id = 3 + 2 * page_count;
    unsigned int const object_count = font_id;
    long *offsets = calloc(object_count + 1, sizeof(long));
    if (!offsets) {
        fclose(f);
        return 1;
    }

    fprintf(f, "%%PDF-1.4\n%%\xE2\xE3\xCF\xD3\n");
    offsets[1] = ftell(f);
    fprintf(f, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");
    offsets[2] = ftell(f);

    fprintf(f, "2 0 obj\n<< /Type /Pages /Kids [");
    for (unsigned int i = 0; i < page_count; i++) {
        fprintf(f, "%u 0 R ", 3 + i);
    }
    fprintf(f, "] /Count %u >>\nendobj\n", page_count);

    for (unsigned int i = 0; i < page_count; i++) {
        unsigned int page_id = 3 + i;
        unsigned int content_id = 3 + page_count + i;
        offsets[page_id] = ftell(f);
        fprintf(
            f,
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
        offsets[content_id] = ftell(f);
        fprintf(f, "%u 0 obj\n<< /Length %lu >>\nstream\n", content_id, pages[i].len);
        fwrite(pages[i].data, 1, pages[i].len, f);
        fprintf(f, "\nendstream\nendobj\n");
    }

    offsets[font_id] = ftell(f);
    fprintf(f, "%u 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n", font_id);
    long xref = ftell(f);
    fprintf(f, "xref\n0 %u\n0000000000 65535 f \n", object_count + 1);
    for (unsigned int i = 1; i <= object_count; i++) {
        fprintf(f, "%010ld 00000 n \n", offsets[i]);
    }
    fprintf(f, "trailer\n<< /Size %u /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", object_count + 1, xref);
    free(offsets);
    fclose(f);
    return 0;
}
