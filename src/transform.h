#ifndef APP_TRANSFORM_H
#define APP_TRANSFORM_H


typedef struct Transform Transform;


Transform *transform_make(void);
void transform_destroy(Transform *t);
Transform *transform_copy(Transform const *t);

double const * transform_getc_matrix(Transform const *t);

void transform_compose(Transform *dst_left, Transform const *src_right);
void transform_compose_list(Transform *dst_left, double const list[6]);
void transform_translate(Transform *t, double tx, double ty);
void transform_rotate(Transform *t, double angle_deg, double cx, double cy);


#endif
