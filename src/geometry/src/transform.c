#include "../include/geometry/transform.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>


#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)


struct Transform {
    double matrix[6];
};


Transform *geo_transform_make(void) {
    Transform *t = (Transform *)malloc(sizeof(Transform));
    if (!t) {
        return NULL;
    }
    *t = (Transform){.matrix={1, 0, 0, 0, 1, 0}};
    return t;
}


void geo_transform_destroy(Transform *t) {
    free(t);
}


Transform *geo_transform_copy(Transform const *t) {
    Transform *result = geo_transform_make();
    if (!result) {
        return NULL;
    }
    memcpy(result, t, sizeof(Transform));
    return result;
};


inline double const * geo_transform_getc_matrix(Transform const *t) {
    return t->matrix;
};


inline Point2D geo_transform_apply_point(Transform const *t, Point2D p) {
    return (Point2D) {
        .x = t->matrix[0] * p.x + t->matrix[1] *p.y + t->matrix[2],
        .y = t->matrix[3] * p.x + t->matrix[4] *p.y + t->matrix[5],
    };
}


void geo_transform_compose_list(Transform *dst, double const list[6]) {
    Transform result = {.matrix={0}};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            result.matrix[3*i + j] = \
                dst->matrix[3*i + 0] * list[3*0 + j]
                + dst->matrix[3*i + 1] * list[3*1 + j]
            ;
            if (j == 2) {
                result.matrix[3*i + j] += dst->matrix[3*i + 2];
            }
        }
    }
    memcpy(dst, &result, sizeof(Transform));
}


void geo_transform_compose(Transform *dst, Transform const *src) {
    geo_transform_compose_list(dst, src->matrix);
}


void geo_transform_translate_val(Transform *t, double tx, double ty) {
    Transform tr = {.matrix={1, 0, tx, 0, 1, ty}};
    geo_transform_compose(t, &tr);
}


void geo_transform_rotate_val(
    Transform *t,
    double angle_deg,
    double cx,
    double cy
) {
    double rad = DEG_TO_RAD(angle_deg);
    double cos_a = cos(rad);
    double sin_a = sin(rad);
    double comp_cos_a = 1.0 - cos_a;

    Transform rotate = {.matrix={
        cos_a, -sin_a, cx * comp_cos_a + cy * sin_a,
        sin_a,  cos_a, cy * comp_cos_a - cx * sin_a
    }};
    geo_transform_compose(t, &rotate);
}


inline void geo_transform_translate(Transform *t, Size2D v) {
    geo_transform_translate_val(t, v.width, v.height);
};


inline void geo_transform_rotate(Transform *t, double angle_deg, Point2D c) {
    geo_transform_rotate_val(t, angle_deg, c.x, c.y);
};
