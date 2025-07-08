#ifndef APP_SRC_SVG_PATH_H
#define APP_SRC_SVG_PATH_H


#include "../geometry.h"


typedef enum {
    SVG_PATH_CMD_UNKNOWN,
    SVG_PATH_CMD_LINE,
    SVG_PATH_CMD_QBEZIER,
    SVG_PATH_CMD_CBEZIER
} SVGPathCommandType;


typedef struct {
    SVGPathCommandType type;
    Point2D start;
    Point2D end;
    Point2D ctrl[2];
} SVGPathCommand;


typedef struct SVGPath SVGPath;


SVGPath *svg_path_make_from_string(char const *s);
void svg_path_destroy(SVGPath *path);
int svg_path_command_count(SVGPath const *path);
SVGPathCommand const *svg_path_get_command(SVGPath const *path, unsigned int index);
void svg_path_apply_transform(SVGPath *path, double matrix[6]);


#endif
