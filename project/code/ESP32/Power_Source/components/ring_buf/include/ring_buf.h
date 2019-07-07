#pragma once

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t *p_o;                /**< Original pointer */
    uint8_t *volatile p_r;       /**< Read pointer */
    uint8_t *volatile p_w;       /**< Write pointer */
    volatile int fill_cnt;       /**< Number of filled slots */
    int    size;                 /**< Buffer size */
} ring_buf_t;

int ring_buf_init(ring_buf_t *r, uint8_t *buf, int size);
int ring_buf_put(ring_buf_t *r, uint8_t c);
int ring_buf_get(ring_buf_t *r, uint8_t *c, int length);

#ifdef __cplusplus
}
#endif