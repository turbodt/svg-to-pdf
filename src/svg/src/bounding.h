#ifndef APP_SVG_BOUNDING_H
#define APP_SVG_BOUNDING_H


#include <geometry.h>
#include "./path.h"


Box2D svg_path_get_bbox(SVGPath const *);
Box2D svg_path_command_get_bbox(SVGPathCommand const *);


#endif
