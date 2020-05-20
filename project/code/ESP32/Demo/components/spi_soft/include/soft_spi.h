#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SOFT_SPI_CLK_GPIO  16
#define SOFT_SPI_MOSI_GPIO 15
#define SOFT_SPI_MISO_GPIO 33
#define SOFT_SPI_CS_GPIO   17

uint8_t soft_spi_trans_byte(uint8_t data);

uint8_t soft_spi_trans_9bits_cmd(uint8_t data);

uint8_t soft_spi_trans_9bits_data(uint8_t data);

void soft_spi_set_cs(uint8_t level);

void soft_spi_init();

#ifdef __cplusplus
}
#endif
