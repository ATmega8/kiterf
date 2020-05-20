/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <sys/errno.h>

#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "soft_spi.h"

static const char *TAG = "soft_spi";

uint8_t soft_spi_trans_byte(uint8_t data)
{
    uint8_t read_data = 0x0;
    for (int x = 0; x < 8; x++) {
        gpio_set_level(SOFT_SPI_CLK_GPIO, 0);
        gpio_set_level(SOFT_SPI_MOSI_GPIO, (data >> (7 - x)) & 0x01);
        ets_delay_us(1);
        gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
        read_data |= gpio_get_level(SOFT_SPI_MISO_GPIO) << (7 - x);
        ets_delay_us(1);
    }

    return read_data;
}

uint8_t soft_spi_trans_9bits_cmd(uint8_t data)
{
    uint8_t read_data = 0x0;
    gpio_set_level(SOFT_SPI_CLK_GPIO, 0);
    gpio_set_level(SOFT_SPI_MOSI_GPIO, 0);
    // ets_delay_us(1);
    gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
    // ets_delay_us(1);
    for (int x = 0; x < 8; x++) {
        gpio_set_level(SOFT_SPI_CLK_GPIO, 0);
        gpio_set_level(SOFT_SPI_MOSI_GPIO, (data >> (7 - x)) & 0x01);
        // ets_delay_us(1);
        gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
        read_data |= gpio_get_level(SOFT_SPI_MISO_GPIO) << (7 - x);
        // ets_delay_us(1);
    }

    return read_data;
}

uint8_t soft_spi_trans_9bits_data(uint8_t data)
{
    uint8_t read_data = 0x0;
    gpio_set_level(SOFT_SPI_CLK_GPIO, 0);
    gpio_set_level(SOFT_SPI_MOSI_GPIO, 1);
    // ets_delay_us(1);
    gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
    // ets_delay_us(1);
    for (int x = 0; x < 8; x++) {
        gpio_set_level(SOFT_SPI_CLK_GPIO, 0);
        gpio_set_level(SOFT_SPI_MOSI_GPIO, (data >> (7 - x)) & 0x01);
        // ets_delay_us(1);
        gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
        read_data |= gpio_get_level(SOFT_SPI_MISO_GPIO) << (7 - x);
        // ets_delay_us(1);
    }

    return read_data;
}

void soft_spi_set_cs(uint8_t level)
{
    gpio_set_level(SOFT_SPI_CS_GPIO, level);
}

void soft_spi_init()
{
    gpio_config_t io_config;
    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pin_bit_mask = (1ULL << SOFT_SPI_CLK_GPIO) | (1ULL << SOFT_SPI_MOSI_GPIO) | (1ULL << SOFT_SPI_CS_GPIO);
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en  = GPIO_PULLUP_DISABLE;

    gpio_config(&io_config);

    io_config.intr_type = GPIO_PIN_INTR_DISABLE;
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = (1ULL << SOFT_SPI_MISO_GPIO);
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en  = GPIO_PULLUP_DISABLE;

    gpio_config(&io_config);

    soft_spi_set_cs(1);
    gpio_set_level(SOFT_SPI_CLK_GPIO, 1);
}