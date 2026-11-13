#include "./points.h"
#include "./double_parser.h"
#include <ctype.h>
#include <stdlib.h>


struct SVGPoints {
    Point2D *points;
    unsigned int count;
    unsigned int capacity;
};


static int svg_points_add(SVGPoints *points, Point2D point);
static char const *skip_separators(char const *s);


SVGPoints *svg_points_make_from_string(char const *s) {
    SVGPoints *points = malloc(sizeof(SVGPoints));
    if (!points) {
        return NULL;
    }
    points->points = NULL;
    points->count = 0;
    points->capacity = 0;

    char const *p = s;
    while (1) {
        p = skip_separators(p);
        if (!*p) {
            break;
        }

        double coords[2];
        char const *next = svg_double_n_parse(p, 2, coords);
        if (next != p) {
            if (svg_points_add(points, (Point2D){coords[0], coords[1]})) {
                goto SVG_POINTS_MAKE_FAILED;
            }
            p = next;
            continue;
        }

        /* SVG 2 requires dropping a trailing odd coordinate. */
        double last;
        next = svg_double_parse(p, &last);
        if (next == p) {
            goto SVG_POINTS_MAKE_FAILED;
        }
        next = skip_separators(next);
        if (*next) {
            goto SVG_POINTS_MAKE_FAILED;
        }
        break;
    }

    return points;

SVG_POINTS_MAKE_FAILED:
    svg_points_destroy(points);
    return NULL;
}


void svg_points_destroy(SVGPoints *points) {
    if (!points) {
        return;
    }
    free(points->points);
    free(points);
}


unsigned int svg_points_count(SVGPoints const *points) {
    return points->count;
}


Point2D svg_points_get(SVGPoints const *points, unsigned int index) {
    if (index >= points->count) {
        return (Point2D){0};
    }
    return points->points[index];
}


void svg_points_apply_transform(SVGPoints *points, Transform const *t) {
    for (unsigned int i = 0; i < points->count; ++i) {
        points->points[i] = geo_transform_apply_point(t, points->points[i]);
    }
}


int svg_points_add(SVGPoints *points, Point2D point) {
    if (points->count < points->capacity) {
        points->points[points->count++] = point;
        return 0;
    }

    unsigned int new_capacity = points->capacity ? 2 * points->capacity : 8;
    Point2D *new_ptr = realloc(
        points->points,
        new_capacity * sizeof(Point2D)
    );
    if (!new_ptr) {
        return 1;
    }
    points->points = new_ptr;
    points->capacity = new_capacity;
    points->points[points->count++] = point;
    return 0;
}


char const *skip_separators(char const *s) {
    while (*s == ',' || isspace((unsigned char)*s)) {
        s++;
    }
    return s;
}
