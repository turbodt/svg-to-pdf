#include "./pdf-writer.h"
#include <stdio.h>


int pdf_write_file(
    char const *filename,
    PdfBuf const *content,
    double width,
    double height
) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        return 1;
    }

    long offsets[6] = {0};
    fprintf(f, "%%PDF-1.4\n%%\xE2\xE3\xCF\xD3\n");
    offsets[1] = ftell(f);
    fprintf(f, "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n");
    offsets[2] = ftell(f);
    fprintf(f, "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n");
    offsets[3] = ftell(f);
    fprintf(
        f,
        "3 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 %.6f %.6f] "
        "/Resources << /Font << /F1 5 0 R >> >> /Contents 4 0 R >>\nendobj\n",
        width,
        height
    );
    offsets[4] = ftell(f);
    fprintf(f, "4 0 obj\n<< /Length %zu >>\nstream\n", content->len);
    fwrite(content->data, 1, content->len, f);
    fprintf(f, "\nendstream\nendobj\n");
    offsets[5] = ftell(f);
    fprintf(f, "5 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n");
    long xref = ftell(f);
    fprintf(f, "xref\n0 6\n0000000000 65535 f \n");
    for (int i = 1; i < 6; i++) {
        fprintf(f, "%010ld 00000 n \n", offsets[i]);
    }
    fprintf(f, "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", xref);
    fclose(f);
    return 0;
}
