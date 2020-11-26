#include "stdio.h"
#ifndef COMPRESS_H
#define COMPRESS_H

int compress_buffer(
        char *src,
        ssize_t src_len,
        char *dest,
        ssize_t dest_len
        );

#endif
