#include "./transform.h"
#include <string.h>
#include <stdio.h>


void svg_transform_perform_operation(Transform *t, const char *svg) {
    if (!svg) return;

    const char *p = svg;
    while (*p) {
        while (*p == ' ' || *p == ',') ++p;

        if (strncmp(p, "translate(", 10) == 0) {
            Size2D v;
            sscanf(p + 10, "%lf %lf", &v.width, &v.height);
            geo_transform_translate(t, v);
            p = strchr(p, ')');
            if (p) {
                ++p;
            }
        } else if (strncmp(p, "rotate(", 7) == 0) {
            double angle = 0;
            Point2D center = {0};
            sscanf(p + 7, "%lf %lf %lf", &angle, &center.x, &center.y);
            geo_transform_rotate(t, angle, center);
            p = strchr(p, ')');
            if (p) {
                ++p;
            }
        } else if (strncmp(p, "matrix(", 7) == 0) {
            double a, b, c, d, e, f;
            sscanf(p + 7, "%lf %lf %lf %lf %lf %lf", &a, &b, &c, &d, &e, &f);
            geo_transform_compose_list(t, (double const[6]){a, c, e, b, d, f});
            p = strchr(p, ')');
            if (p) {
                ++p;
            }
        } else {
            break; // Unknown or unsupported transform
        }
    }
}


inline Point2D svg_transform_apply(Transform const *t, Point2D a) {
    double const * const m = geo_transform_getc_matrix(t);
    return (Point2D) {
        .x=m[0]*a.x + m[1]*a.y + m[2],
        .y=m[3]*a.x + m[4]*a.y + m[5],
    };
};
