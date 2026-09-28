#include "compat.h"


int xml2_wasi_dup(int fd) {
    (void) fd;
    return -1;
}
