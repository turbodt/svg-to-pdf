#include "./path.h"
#include "./double_parser.h"
#include <geometry/transform.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


struct SVGPath {
    SVGPathCommand *commands;
    unsigned int count;
    unsigned int capacity;
};


static int parse_origin(
    char cmd_char,
    Point2D p_prev,
    char const **p,
    Point2D *origin
);
static int parse_command(
    char cmd_char,
    Point2D origin,
    Point2D cp_prev_q,
    Point2D cp_prev_c,
    Point2D p_prev,
    char const **p,
    SVGPathCommand *out
);
static int parse_arc(
    int relative,
    Point2D p_prev,
    char const **p,
    SVGPathCommand *out
);
static Point2D ellipse_arc_point(
    double cx,
    double cy,
    double rx,
    double ry,
    double cos_phi,
    double sin_phi,
    double u,
    double v
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
    path->commands = NULL;

    Point2D point_origin = {0};
    Point2D point_prev = {0};
    Point2D point_ctrl_prev_q = {0};
    Point2D point_ctrl_prev_c = {0};
    char last_cmd_char = 'Z';
    while (*p) {
        advance_separators(&p);
        if (!*p) {
            break;
        }
        switch (*p) {
            case 'M':case 'm': {
                last_cmd_char = *p;
                p++;
                if (
                    parse_origin(last_cmd_char, point_prev, &p, &point_origin)
                ) {
                    goto SVG_PATH_MAKE_PARSING_FAILED;
                };
                point_prev = point_origin;
                point_ctrl_prev_q = point_origin;
                point_ctrl_prev_c = point_origin;
                /* Extra coordinate pairs after a moveto are implicit
                 * lineto commands, relative when the moveto is. */
                advance_separators(&p);
                double peek;
                if (svg_double_parse(p, &peek) == p) {
                    continue;
                }
                last_cmd_char = last_cmd_char == 'm' ? 'l' : 'L';
            } break;
            case 'Z':case 'z':
            case 'L':case 'l':case 'H':case 'h':case 'V':case 'v':
            case 'Q':case 'q':case 'T':case 't':
            case 'C':case 'c':case 'S':case 's':
            case 'A':case 'a':
                last_cmd_char = *p;
                p++;
            break;
        }
        /* Arc commands expand into up to four bezier commands. */
        SVGPathCommand commands[4] = {0};
        int const count = parse_command(
            last_cmd_char,
            point_origin,
            point_ctrl_prev_q,
            point_ctrl_prev_c,
            point_prev,
            &p,
            commands
        );
        if (count < 0) {
            goto SVG_PATH_MAKE_PARSING_FAILED;
        };
        for (int i = 0; i < count; ++i) {
            if (svg_path_add_command(path, &commands[i])) {
                goto SVG_PATH_MAKE_PARSING_FAILED;
            };
        };
        if (count > 0) {
            point_prev = commands[count-1].end;
            /* The command letter decides how smooth curve reflection
             * carries over. Arcs decompose into bezier commands, but
             * T and S after an arc fall back to the current point. */
            switch (last_cmd_char) {
                case 'Q':case 'q':case 'T':case 't': {
                    point_ctrl_prev_q = commands[count-1].ctrl[0];
                    point_ctrl_prev_c = commands[count-1].end;
                } break;
                case 'C':case 'c':case 'S':case 's': {
                    point_ctrl_prev_q = commands[count-1].end;
                    point_ctrl_prev_c = commands[count-1].ctrl[1];
                } break;
                default: {
                    point_ctrl_prev_q = commands[count-1].end;
                    point_ctrl_prev_c = commands[count-1].end;
                } break;
            };
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


void svg_path_apply_transform(SVGPath *path, Transform const *t) {
    for (unsigned int i = 0; i < path->count; ++i) {
        SVGPathCommand *c = &path->commands[i];
        Point2D * points[4] = {
            &c->start,
            &c->end,
            &c->ctrl[0],
            &c->ctrl[1]
        };
        unsigned int n = c->type == SVG_PATH_CMD_LINE ? 2
            : c->type == SVG_PATH_CMD_QBEZIER ? 3
            : c->type == SVG_PATH_CMD_CBEZIER ? 4
            : 0;

        for (unsigned int j = 0; j < n; ++j) {
            Point2D *point = points[j];
            *point = geo_transform_apply_point(t, *point);
        }
    }
}


int parse_origin(
    char cmd_char,
    Point2D p_prev,
    char const **p,
    Point2D *origin
) {
    double coords[2];
    int is_relative = cmd_char >= 'a' && cmd_char <= 'z';
    char const *new_ptr = svg_double_n_parse(*p, 2, coords);
    if (new_ptr == *p) {
        return 1;
    }
    *p = new_ptr;
    origin->x = coords[0];
    origin->y = coords[1];
    if (is_relative) {
        point_add(origin, p_prev);
    }
    return 0;
};


int parse_command(
    char cmd_char,
    Point2D origin,
    Point2D cp_prev_q,
    Point2D cp_prev_c,
    Point2D p_prev,
    char const **p,
    SVGPathCommand *out
) {
    SVGPathCommand command = {
        .type = SVG_PATH_CMD_UNKNOWN,
    };

    int relative = cmd_char >= 'a' && cmd_char <= 'z';
    if (relative) {
        cmd_char -= ('a' - 'A');
    }

    double coords[6];

    command.start = p_prev;
    switch (cmd_char) {
        case 'L': {
            char const *new_ptr = svg_double_n_parse(*p, 2, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[0];
            command.end.y = coords[1];
            if (relative) {
                point_add(&command.end, p_prev);
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'H': {
            char const *new_ptr = svg_double_n_parse(*p, 1, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[0];
            command.end.y = p_prev.y;
            if (relative) {
                command.end.x += p_prev.x;
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'V': {
            char const *new_ptr = svg_double_n_parse(*p, 1, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.y = coords[0];
            command.end.x = p_prev.x;
            if (relative) {
                command.end.y += p_prev.y;
            }
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'Z': {
            command.start = p_prev;
            command.end = origin;
            command.type = SVG_PATH_CMD_LINE;
        } break;
        case 'Q': {
            char const *new_ptr = svg_double_n_parse(*p, 4, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[2];
            command.end.y = coords[3];
            command.ctrl[0].x = coords[0];
            command.ctrl[0].y = coords[1];
            if (relative) {
                point_add(&command.end, p_prev);
                point_add(&command.ctrl[0], p_prev);
            }
            command.type = SVG_PATH_CMD_QBEZIER;
        } break;
        case 'T': {
            char const *new_ptr = svg_double_n_parse(*p, 2, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[0];
            command.end.y = coords[1];
            command.ctrl[0].x = 2*command.start.x - cp_prev_q.x;
            command.ctrl[0].y = 2*command.start.y - cp_prev_q.y;
            if (relative) {
                point_add(&command.end, p_prev);
            }
            command.type = SVG_PATH_CMD_QBEZIER;
        } break;
        case 'C': {
            char const *new_ptr = svg_double_n_parse(*p, 6, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[4];
            command.end.y = coords[5];
            command.ctrl[1].x = coords[2];
            command.ctrl[1].y = coords[3];
            command.ctrl[0].x = coords[0];
            command.ctrl[0].y = coords[1];
            if (relative) {
                point_add(&command.end, p_prev);
                point_add(&command.ctrl[1], p_prev);
                point_add(&command.ctrl[0], p_prev);
            }
            command.type = SVG_PATH_CMD_CBEZIER;
        } break;
        case 'S': {
            char const *new_ptr = svg_double_n_parse(*p, 4, coords);
            if (new_ptr == *p) {
                goto SVG_PATH_PARSING_FAILED;
            }
            *p = new_ptr;
            command.end.x = coords[2];
            command.end.y = coords[3];
            command.ctrl[1].x = coords[0];
            command.ctrl[1].y = coords[1];
            command.ctrl[0].x = 2*command.start.x - cp_prev_c.x;
            command.ctrl[0].y = 2*command.start.y - cp_prev_c.y;
            if (relative) {
                point_add(&command.end, p_prev);
                point_add(&command.ctrl[1], p_prev);
            }
            command.type = SVG_PATH_CMD_CBEZIER;
        } break;
        case 'A': {
            return parse_arc(relative, p_prev, p, out);
        }
        default: break;
    };
    if (command.type == SVG_PATH_CMD_UNKNOWN) {
        goto SVG_PATH_PARSING_FAILED;
    }
    out[0] = command;
    return 1;
SVG_PATH_PARSING_FAILED:
    return -1;
};


Point2D ellipse_arc_point(
    double cx,
    double cy,
    double rx,
    double ry,
    double cos_phi,
    double sin_phi,
    double u,
    double v
) {
    double const x = rx*u;
    double const y = ry*v;
    Point2D point;
    point.x = cx + x*cos_phi - y*sin_phi;
    point.y = cy + x*sin_phi + y*cos_phi;
    return point;
};


int parse_arc(
    int relative,
    Point2D p_prev,
    char const **p,
    SVGPathCommand *out
) {
    double radii_rotation[3];
    char const *new_ptr = svg_double_n_parse(*p, 3, radii_rotation);
    if (new_ptr == *p) {
        return -1;
    }
    *p = new_ptr;

    /* The arc flags are single '0' or '1' characters, optionally
     * separated, as in the path data grammar. */
    int flags[2] = {0, 0};
    for (unsigned int i = 0; i < 2; ++i) {
        advance_separators(p);
        if (**p != '0' && **p != '1') {
            return -1;
        }
        flags[i] = **p - '0';
        (*p)++;
    }

    double endpoint[2];
    new_ptr = svg_double_n_parse(*p, 2, endpoint);
    if (new_ptr == *p) {
        return -1;
    }
    *p = new_ptr;

    Point2D end;
    end.x = endpoint[0];
    end.y = endpoint[1];
    if (relative) {
        point_add(&end, p_prev);
    }

    /* Arcs with identical endpoints are omitted entirely. */
    if (end.x == p_prev.x && end.y == p_prev.y) {
        return 0;
    }

    double rx = fabs(radii_rotation[0]);
    double ry = fabs(radii_rotation[1]);

    /* Arcs with a zero radius degenerate to a straight line. */
    if (rx == 0.0 || ry == 0.0) {
        out[0].type = SVG_PATH_CMD_LINE;
        out[0].start = p_prev;
        out[0].end = end;
        return 1;
    }

    double const phi = radii_rotation[2]*M_PI/180.0;
    double const cos_phi = cos(phi);
    double const sin_phi = sin(phi);

    /* Endpoint to center parameterization conversion. */
    double const dx_half = (p_prev.x - end.x)/2.0;
    double const dy_half = (p_prev.y - end.y)/2.0;
    double const x1p = cos_phi*dx_half + sin_phi*dy_half;
    double const y1p = -sin_phi*dx_half + cos_phi*dy_half;

    /* Scale up radii too small to reach the endpoint. */
    double const lambda = x1p*x1p/(rx*rx) + y1p*y1p/(ry*ry);
    if (lambda > 1.0) {
        double const scale = sqrt(lambda);
        rx *= scale;
        ry *= scale;
    }

    double numerator = rx*rx*ry*ry - rx*rx*y1p*y1p - ry*ry*x1p*x1p;
    double const denominator = rx*rx*y1p*y1p + ry*ry*x1p*x1p;
    if (numerator < 0.0) {
        numerator = 0.0;
    }
    double const coef = (flags[0] != flags[1] ? 1.0 : -1.0)*sqrt(
        numerator/denominator
    );
    double const cxp = coef*rx*y1p/ry;
    double const cyp = -coef*ry*x1p/rx;
    double const cx = cos_phi*cxp - sin_phi*cyp + (p_prev.x + end.x)/2.0;
    double const cy = sin_phi*cxp + cos_phi*cyp + (p_prev.y + end.y)/2.0;

    double const theta_1 = atan2((y1p - cyp)/ry, (x1p - cxp)/rx);
    double const theta_2 = atan2((-y1p - cyp)/ry, (-x1p - cxp)/rx);

    double delta_theta = theta_2 - theta_1;
    if (!flags[1] && delta_theta > 0.0) {
        delta_theta -= 2.0*M_PI;
    }
    if (flags[1] && delta_theta < 0.0) {
        delta_theta += 2.0*M_PI;
    }

    /* Split the arc into cubic beziers of at most a quarter turn. */
    unsigned int segments = (unsigned int)ceil(fabs(delta_theta)/(M_PI/2.0));
    if (segments < 1) {
        return 0;
    }
    if (segments > 4) {
        segments = 4;
    }

    double const delta = delta_theta/(double)segments;
    double const alpha = (4.0/3.0)*tan(delta/4.0);

    for (unsigned int i = 0; i < segments; ++i) {
        double const theta = theta_1 + (double)i*delta;
        double const theta_next = theta_1 + (double)(i + 1)*delta;

        /* Cubic bezier control points on the unit circle, following
         * the tangent at each endpoint. */
        double const u0 = cos(theta);
        double const v0 = sin(theta);
        double const u3 = cos(theta_next);
        double const v3 = sin(theta_next);
        double const u1 = u0 - alpha*v0;
        double const v1 = v0 + alpha*u0;
        double const u2 = u3 + alpha*v3;
        double const v2 = v3 - alpha*u3;

        SVGPathCommand *command = &out[i];
        command->type = SVG_PATH_CMD_CBEZIER;
        command->start = ellipse_arc_point(
            cx, cy, rx, ry, cos_phi, sin_phi, u0, v0
        );
        command->ctrl[0] = ellipse_arc_point(
            cx, cy, rx, ry, cos_phi, sin_phi, u1, v1
        );
        command->ctrl[1] = ellipse_arc_point(
            cx, cy, rx, ry, cos_phi, sin_phi, u2, v2
        );
        command->end = ellipse_arc_point(
            cx, cy, rx, ry, cos_phi, sin_phi, u3, v3
        );
    }
    /* Snap the arc endpoints to the parsed path points. */
    out[0].start = p_prev;
    out[segments-1].end = end;

    return (int)segments;
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
    while (**p == ',' || isspace((unsigned char)**p)) {
        (*p)++;
        count++;
    }
    return count;
};


inline void point_add(Point2D *dst, Point2D src) {
    dst->x += src.x;
    dst->y += src.y;
};
