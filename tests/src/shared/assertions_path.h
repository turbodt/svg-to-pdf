#ifndef TESTS_SHARED_ASSERTIONS_PATH_H
#define TESTS_SHARED_ASSERTIONS_PATH_H


#include "./config.h"
#include "./assertions_basic.h"
#include <math.h>


#define ASSERT_FLOAT_EQ(a, b, ...) ASSERT(fabs(a-b) < 1e-10, ## __VA_ARGS__)
#define ASSERT_POINT_EQ(a, b, ...) do {\
    ASSERT_FLOAT_EQ((a).x, (b).x, ## __VA_ARGS__); \
    ASSERT_FLOAT_EQ((a).y, (b).y, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_PATH_LINE_IS(command, start, end, ...) do {\
        ASSERT_EQ((command).type, SVG_PATH_CMD_LINE, ## __VA_ARGS__); \
        ASSERT_POINT_EQ((command).start, start, ## __VA_ARGS__); \
        ASSERT_POINT_EQ((command).end, end, ## __VA_ARGS__); \
    } while(0)


#endif
