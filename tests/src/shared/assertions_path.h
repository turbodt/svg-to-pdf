#ifndef TESTS_SHARED_ASSERTIONS_PATH_H
#define TESTS_SHARED_ASSERTIONS_PATH_H


#include "./config.h"
#include "./assertions_basic.h"
#include <math.h>


#define ASSERT_FLOAT_EQ(a, b, ...) ASSERT(fabs((double)(a)-(double)(b)) < 1e-10, ## __VA_ARGS__)
#define ASSERT_POINT_EQ(a, b, ...) do {\
    ASSERT_FLOAT_EQ((a).x, (b).x, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).y, (b).y, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_PATH_IS_LINE(command, sx, sy, ex, ey, ...) do {\
        ASSERT_EQ((command).type, SVG_PATH_CMD_LINE, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.x, sx, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.y, sy, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.x, ex, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.y, ey, ## __VA_ARGS__); \
    } while(0)


#endif
