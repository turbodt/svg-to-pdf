#ifndef APP_SVG_BOUNDING_H
#define APP_SVG_BOUNDING_H


#include <geometry.h>
#include "./path.h"
#include "./points.h"


Box2D svg_path_get_bbox(SVGPath const *);
Box2D svg_path_command_get_bbox(SVGPathCommand const *);
Box2D svg_points_get_bbox(SVGPoints const *);


#endif
