#ifndef TESTS_SHARED_ASSERTIONS_PATH_H
#define TESTS_SHARED_ASSERTIONS_PATH_H


#include "./config.h"
#include "./assertions_basic.h"
#include <math.h>


#define ASSERT_PATH_IS_UNKNOWN(command, ...) \
        ASSERT_EQ((command).type, SVG_PATH_CMD_UNKNOWN, ## __VA_ARGS__)
#define ASSERT_PATH_IS_LINE(command, sx, sy, ex, ey, ...) do {\
        ASSERT_EQ((command).type, SVG_PATH_CMD_LINE, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.x, sx, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.y, sy, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.x, ex, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.y, ey, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_PATH_IS_QBEZIER(command, sx, sy, cx, cy, ex, ey, ...) do {\
        ASSERT_EQ((command).type, SVG_PATH_CMD_QBEZIER, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.x, sx, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.y, sy, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[0].x, cx, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[0].y, cy, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.x, ex, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.y, ey, ## __VA_ARGS__); \
    } while(0)
#define ASSERT_PATH_IS_CBEZIER(command, sx, sy, c1x, c1y, c2x, c2y, ex, ey, ...) do {\
        ASSERT_EQ((command).type, SVG_PATH_CMD_CBEZIER, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.x, sx, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).start.y, sy, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[0].x, c1x, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[0].y, c1y, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[1].x, c2x, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).ctrl[1].y, c2y, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.x, ex, ## __VA_ARGS__); \
        ASSERT_FLOAT_EQ((command).end.y, ey, ## __VA_ARGS__); \
    } while(0)


#endif
