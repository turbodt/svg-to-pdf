#ifndef TESTS_SHARED_ASSERTIONS_GEOMETRY_H
#define TESTS_SHARED_ASSERTIONS_GEOMETRY_H


#include "./config.h"
#include "./assertions_basic.h"
#include <math.h>


#define ASSERT_POINT_EQ(a, b, ...) do {\
    ASSERT_FLOAT_EQ((a).x, (b).x, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).y, (b).y, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_BOX_EQ(a, x, y, w, h, ...) do {\
    ASSERT_FLOAT_EQ((a).tl.x, x, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).tl.y, y, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).size.width, w, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).size.height, h, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_BOX_COORDS_EQ(a, x1, y1, x2, y2, ...) do {\
    ASSERT_FLOAT_EQ((a).tl.x, x1, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).tl.y, y1, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).size.width, x2 - x1, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).size.height, y2 - y1, ## __VA_ARGS__); \
    } while(0)


#endif
