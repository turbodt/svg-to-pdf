#ifndef APP_SVG_TRANSFORM_H
#define APP_SVG_TRANSFORM_H


#include <geometry/transform.h>


void svg_transform_perform_operation(Transform *t, char const *svg);
Point2D svg_transform_apply(Transform const *t, Point2D a);


#endif
