#include "../include/geometry.h"


static void box2d_swap(Box2D *, Box2D *);
static void point2d_swap(Point2D *, Point2D *);
static void size2d_swap(Size2D *, Size2D *);
static double min4(double[4]);
static double max4(double[4]);


int geo_boxes_intersect(Box2D a, Box2D b) {
    if (a.tl.x > b.tl.x) {
        return geo_boxes_intersect(b, a);
    }
    if (a.tl.x + a.size.width <= b.tl.x) {
        return 0;
    }

    if (a.tl.y > b.tl.x) {
        box2d_swap(&a, &b);
    }
    return a.tl.y + a.size.height > b.tl.y;
};


Box2D geo_boxes_compose(Box2D a, Box2D b) {
    double x_min, x_max, y_min, y_max;
    double xs[4] = {
        a.tl.x, a.tl.x + a.size.width,
        b.tl.x, b.tl.x + b.size.width
    };
    double ys[4] = {
        a.tl.y, a.tl.y + a.size.height,
        b.tl.y, b.tl.y + b.size.height
    };
    x_min = min4(xs);
    x_max = max4(xs);
    y_min = min4(ys);
    y_max = max4(ys);

    return (Box2D){
        .tl = {.x=x_min, .y=y_min},
        .size = {.width=x_max - x_min, .height=y_max - y_min},
    };
};


inline void box2d_swap(Box2D *a, Box2D *b) {
    point2d_swap(&a->tl, &b->tl);
    size2d_swap(&a->size, &b->size);
}


inline void point2d_swap(Point2D *a, Point2D *b) {
    double d = a->x;
    a->x = b->x;
    b->x = d;
    d = a->y;
    a->y = b->y;
    b->y = d;
};


inline void size2d_swap(Size2D *a, Size2D *b) {
    double d = a->width;
    a->width = b->width;
    b->width = d;
    d = a->height;
    a->height = b->height;
    b->height = d;
};


inline double min4(double values[4]) {
    double m = values[0];
    if (m > values[1]) {
        m = values[1];
    }
    if (m > values[2]) {
        m = values[2];
    }
    if (m > values[3]) {
        m = values[3];
    }
    return m;
};

inline double max4(double values[4]) {
    double m = values[0];
    if (m < values[1]) {
        m = values[1];
    }
    if (m < values[2]) {
        m = values[2];
    }
    if (m < values[3]) {
        m = values[3];
    }
    return m;
};
