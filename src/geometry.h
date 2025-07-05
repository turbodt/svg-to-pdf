#ifndef APP_GEOMETRY_H
#define APP_GEOMETRY_H


typedef struct Point2D {
    double x;
    double y;
} Point2D;


typedef struct Size2D {
    double width;
    double height;
} Size2D;


typedef struct Box2D {
    Point2D tl;
    Size2D size;
} Box2D;


int geo_boxes_intersect(Box2D, Box2D);
Box2D geo_boxes_compose(Box2D, Box2D);


#endif
