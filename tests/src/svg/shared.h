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

#define LOG_BOX(box) LOG( \
    "[tl:(%.2f, %.2f) size:(%.2f, %.2f) br:(%.2f, %.2f)]", \
    box.tl.x, box.tl.y, \
    box.size.width, box.size.height, \
    box.tl.x + box.size.width, box.tl.y + box.size.height \
)

#endif
