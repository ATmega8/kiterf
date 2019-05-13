#pragma once

#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_PIN_MISO 25
#define LCD_PIN_MOSI 23
#define LCD_PIN_CLK  19
#define LCD_PIN_CS   22

#define LCD_PIN_DC   21
#define LCD_PIN_RST  18
#define LCD_PIN_BCKL 5

#define LCD_HOR_WIDTH (320)
#define LCD_VER_WIDTH (240)

#define LCD_BURST_MAX_LEN (LCD_HOR_WIDTH * 4)  // Maximum pixel data transferred at a time

void inline lcd_set_res(uint8_t state)
{
    gpio_set_level(LCD_PIN_RST, state);
}

void inline lcd_set_dc(uint8_t state)
{
    gpio_set_level(LCD_PIN_DC, state);
}

void inline lcd_set_blk(uint8_t state)
{
    gpio_set_level(LCD_PIN_BCKL, !state);
}

void lcd_rst();

void lcd_init();

void lcd_write_data(uint16_t *data, int len);

void lcd_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end);

void lcd_clear(uint16_t color);

#ifdef __cplusplus
}
#endif