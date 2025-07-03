#include "./transform.h"
#include "./math.h"
#include <string.h>
#include <stdlib.h>


#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)

struct Transform {
    double matrix[6];
};


Transform *transform_make(void) {
    Transform *t = (Transform *)malloc(sizeof(Transform));
    if (!t) {
        return NULL;
    }
    *t = (Transform){.matrix={1, 0, 0, 0, 1, 0}};
    return t;
}


void transform_destroy(Transform *t) {
    free(t);
}


Transform *transform_copy(Transform const *t) {
    Transform *result = transform_make();
    if (!result) {
        return NULL;
    }
    memcpy(result, t, sizeof(Transform));
    return result;
};


inline double const * transform_getc_matrix(Transform const *t) {
    return t->matrix;
};


void transform_compose_list(Transform *dst, double const list[6]) {
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


void transform_compose(Transform *dst, Transform const *src) {
    transform_compose_list(dst, src->matrix);
}


void transform_translate(Transform *t, double tx, double ty) {
    Transform tr = {.matrix={1, 0, tx, 0, 1, ty}};
    transform_compose(t, &tr);
}


void transform_rotate(
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
    transform_compose(t, &rotate);
}
