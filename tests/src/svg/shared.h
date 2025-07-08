#ifndef TESTS_SVG_SHARED_H
#define TESTS_SVG_SHARED_H


#include "./include/test-svg.h"

#define LOG_PATH_COMMAND(command) LOG( \
    "{%c (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f) (%.2f, %.2f)}", \
    "ULQC"[(command).type], \
    (command).start.x, (command).start.y, \
    (command).ctrl[0].x, (command).ctrl[0].y, \
    (command).ctrl[1].x, (command).ctrl[1].y, \
    (command).end.x, (command).end.y \
)

#endif
