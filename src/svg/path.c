#include "./path.h"
#include "./double_parser.h"
#include "./transform.h"
#include "../geometry.h"
#include <stdlib.h>
#include <string.h>


struct SVGPath {
    SVGPathCommand *commands;
    unsigned int count;
    unsigned int capacity;
};


static int parse_origin(char const **p, Point2D *origin);
static SVGPathCommand parse_command(
    char cmd_char,
    Point2D origin,
    Point2D cp_prev,
    Point2D p_prev,
    char const **p
);
static int svg_path_add_command(SVGPath *path, SVGPathCommand const *cmd);
static unsigned int advance_separators(char const **p);
static void point_add(Point2D *dst, Point2D src);


SVGPath *svg_path_make_from_string(char const *p) {
    SVGPath *path = malloc(sizeof(SVGPath));
    if (!path) {
        return NULL;
    }

    path->capacity = 0;
    path->count = 0;

    Point2D point_origin = {0};
    Point2D point_prev = {0};
    Point2D point_ctrl_prev = {0};
    char last_cmd_char = 'Z';
    while (*p) {
        advance_separators(&p);
        switch (*p) {
            case 'M':case 'm': {
                last_cmd_char = *p;
                p++;
                if (parse_origin(&p, &point_origin)) {
                    goto SVG_PATH_MAKE_PARSING_FAILED;
                };
                point_prev = point_origin;
                point_ctrl_prev = point_origin;
                continue;
            } break;
            case 'Z':case 'z':
            case 'L':case 'l':case 'H':case 'h':case 'V':case 'v':
            case 'Q':case 'q':case 'T':case 't':
            case 'C':case 'c':case 'S':case 's':
                last_cmd_char = *p;
                p++;
            break;
            default:
                goto SVG_PATH_MAKE_PARSING_FAILED;
            break;
        }
        SVGPathCommand command = parse_command(
            last_cmd_char,
            point_origin,
            point_ctrl_prev,
            point_prev,
            &p
        );
        switch(command.type) {
            case SVG_PATH_CMD_UNKNOWN:
                goto SVG_PATH_MAKE_PARSING_FAILED;
            case SVG_PATH_CMD_CBEZIER:
            case SVG_PATH_CMD_QBEZIER:
                point_ctrl_prev = command.points[2];
            case SVG_PATH_CMD_LINE:
                point_prev = command.points[1];
        };
        if (svg_path_add_command(path, &command)) {
            goto SVG_PATH_MAKE_PARSING_FAILED;
        };
    }

    return path;

SVG_PATH_MAKE_PARSING_FAILED:
    svg_path_destroy(path);
    return NULL;
}

void svg_path_destroy(SVGPath *path) {
    if (!path) {
        return;
    }
    free(path->commands);
    free(path);
}


inline int svg_path_command_count(SVGPath const *path) {
    return path->count;
}


SVGPathCommand const *svg_path_get_command(
    SVGPath const *path,
    unsigned int index
) {
    if (index >= path->count) {
        return NULL;
    }
    return &path->commands[index];
}


void svg_path_apply_transform(SVGPath *path, double matrix[6]) {
    for (int i = 0; i < path->count; ++i) {
        SVGPathCommand *c = &path->commands[i];
        unsigned int n = c->type == SVG_PATH_CMD_LINE ? 2
            : c->type == SVG_PATH_CMD_QBEZIER ? 3
            : c->type == SVG_PATH_CMD_CBEZIER ? 4
            : 0;

        for (unsigned int j = 0; j < n; ++j) {
            Point2D *point = &c->points[j];
            point->x  = matrix[0] * point->x + matrix[1] * point->y + matrix[2];
            point->y  = matrix[3] * point->x + matrix[4] * point->y + matrix[5];
        }
    }
}


int parse_origin(char const **p, Point2D *origin) {
    double coords[2];
    char const *new_ptr = svg_double_n_parse(*p, 2, coords);
    if (new_ptr == *p) {
        return 1;
    }
    origin->x = coords[0];
    origin->y = coords[1];
    return 0;
};


SVGPathCommand parse_command(
    char cmd_char,
    Point2D origin,
    Point2D cp_prev,
    Point2D p_prev,
    char const **p
) {
    SVGPathCommand command = {
        .type = SVG_PATH_CMD_UNKNOWN,
    };

    int relative = cmd_char >= 'a' && cmd_char <= 'z';
    if (relative) {
        cmd_char -= ('a' - 'A');
    }

    double coords[6];

    command.points[0] = p_prev;
    switch (cmd_char) {
        case 'L': {
            char const *new_ptr = svg_double_n_parse(*p, 2, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            // end point
            command.points[1].x = coords[0];
            command.points[1].y = coords[1];
            if (relative) {
                point_add(&command.points[1], p_prev);
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'H': {
            char const *new_ptr = svg_double_n_parse(*p, 1, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            // end point
            command.points[1].x = coords[0];
            command.points[1].y = p_prev.y;
            if (relative) {
                command.points[1].x += p_prev.x;
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'V': {
            char const *new_ptr = svg_double_n_parse(*p, 1, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            // end point
            command.points[1].y = coords[0];
            command.points[1].x = p_prev.x;
            if (relative) {
                command.points[1].y += p_prev.y;
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'Z': {
            command.points[0] = p_prev;
            // end point
            command.points[1] = origin;
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'Q': {
            char const *new_ptr = svg_double_n_parse(*p, 4, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            // end point
            command.points[1].x = coords[2];
            command.points[1].y = coords[3];
            // last control point
            command.points[2].x = coords[0];
            command.points[2].y = coords[1];
            if (relative) {
                point_add(&command.points[1], p_prev);
                point_add(&command.points[2], p_prev);
            }
            command.type = SVG_PATH_CMD_QBEZIER;
        } break;
        case 'C': {
            char const *new_ptr = svg_double_n_parse(*p, 6, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            // end point
            command.points[1].x = coords[4];
            command.points[1].y = coords[5];
            // last control point
            command.points[2].x = coords[2];
            command.points[2].y = coords[3];
            // first control point
            command.points[3].x = coords[0];
            command.points[3].y = coords[1];
            if (relative) {
                point_add(&command.points[1], p_prev);
                point_add(&command.points[2], p_prev);
                point_add(&command.points[3], p_prev);
            }
            command.type = SVG_PATH_CMD_CBEZIER;
        } break;
        default: break;
    };
SVG_PATH_PARSING_FAILED:
    return command;
};


int svg_path_add_command(SVGPath *path, SVGPathCommand const *cmd) {
    if (path->count < path->capacity) {
        path->commands[path->count++] = *cmd;
        return 0;
    }

    SVGPathCommand *new_ptr;
    unsigned int new_capacity = path->capacity ? 2 * path->capacity : 8;
    if (path->capacity == 0) {
        new_ptr = malloc(new_capacity * sizeof(SVGPathCommand));
    } else {
        new_ptr = realloc(
            path->commands,
            new_capacity * sizeof(SVGPathCommand)
        );
    }
    if (!new_capacity) {
        return 1;
    }
    path->commands = new_ptr;
    path->capacity = new_capacity;
    return svg_path_add_command(path, cmd);
}


inline unsigned int advance_separators(char const **p) {
    unsigned int count = 0;
    while (**p == ' ' || **p == ',') {
        (*p)++;
        count++;
    }
    return count;
};


inline void point_add(Point2D *dst, Point2D src) {
    dst->x += src.x;
    dst->y += src.y;
};
