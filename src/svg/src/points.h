#ifndef APP_SRC_SVG_POINTS_H
#define APP_SRC_SVG_POINTS_H


#include <geometry/transform.h>


typedef struct SVGPoints SVGPoints;


SVGPoints *svg_points_make_from_string(char const *s);
void svg_points_destroy(SVGPoints *points);
unsigned int svg_points_count(SVGPoints const *points);
Point2D svg_points_get(SVGPoints const *points, unsigned int index);
void svg_points_apply_transform(SVGPoints *points, Transform const *t);


#endif
