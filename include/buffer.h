/**
 * @file buffer.h
 * @brief File implementing a simple buffer
 */
#ifndef __BUFFER_INC
#define __BUFFER_INC

#include <stdint.h>

/* RFC 791: "hosts must be prepared to accept datagrams of up to 576 octets" */
#define BUFFER_SIZE 576

struct buffer {
    /* the actual array of characters */
    unsigned char buf[BUFFER_SIZE];

    /* the current offset into the buffer used (wraps around past BUFFER_SIZE) */
    uint16_t idx;

    /* number of bytes used in the buffer */
    uint16_t used;
};

/* all of the buffers that exist; total = 3KiB used for tx/rx buffers */
__volatile extern struct buffer uart0rx, uart0tx, uart1rx, uart1tx, i2crx, i2ctx;

#endif /* __BUFFER_INC */
