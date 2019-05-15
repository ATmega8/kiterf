#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int flash_buf_read(size_t addr, const void *data, size_t size);

int flash_buf_write(size_t addr, const void *data, size_t size);

int flash_buf_erase();

int flash_buf_get_size();

int flash_buf_deinit();

int flash_buf_init();

#ifdef __cplusplus
}
#endif