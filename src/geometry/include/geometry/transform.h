#ifndef APP_GEOMETRY_TRANSFORM_H
#define APP_GEOMETRY_TRANSFORM_H


#include "../geometry.h"


typedef struct Transform Transform;


Transform *geo_transform_make(void);
void geo_transform_destroy(Transform *t);
Transform *geo_transform_copy(Transform const *t);

double const * geo_transform_getc_matrix(Transform const *t);

Point2D geo_transform_apply_point(Transform const *t, Point2D p);
void geo_transform_compose(Transform *dst_left, Transform const *src_right);
void geo_transform_compose_list(Transform *dst_left, double const list[6]);
void geo_transform_translate(Transform *t, Size2D v);
void geo_transform_translate_val(Transform *t, double tx, double ty);
void geo_transform_rotate(Transform *t, double angle_deg, Point2D center);
void geo_transform_rotate_val(
    Transform *t,
    double angle_deg,
    double cx,
    double cy
);


#endif
