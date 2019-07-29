
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "lcd.h"

static spi_device_handle_t spi;
static SemaphoreHandle_t lcd_write_mux = NULL;
static uint8_t lcd_dc_state = 0;
static uint8_t lcd_num = 0;

#define SPI_BURST_MAX_LEN (240 * 240 * 2)  // Maximum pixel data transferred at a time

static void IRAM_ATTR spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    lcd_set_dc(lcd_dc_state);
}

static void spi_write_data(uint16_t *data, size_t len)
{
    if (len <= 0) {
        return;
    }
    int x, y;
    spi_transaction_t t, *rtrans;
    memset(&t, 0, sizeof(t));         //Zero out the transaction              
    for (y = 0; y < len / SPI_BURST_MAX_LEN; y++) {
        t.tx_buffer = data;
        t.length = 8 * SPI_BURST_MAX_LEN; 
        spi_device_queue_trans(spi, &t, portMAX_DELAY);
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
        data += SPI_BURST_MAX_LEN / 2;
    }
    if (len % SPI_BURST_MAX_LEN) {
        t.tx_buffer = data;
        t.length = 8 * (len % SPI_BURST_MAX_LEN); 
        spi_device_queue_trans(spi, &t, portMAX_DELAY);
        spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    }
}


static void lcd_delay_ms(uint32_t time)
{
    vTaskDelay(time / portTICK_RATE_MS);
}

void lcd_select(uint8_t num)
{
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    switch (num) {
        case 0: {
            lcd_set_cs1(1);
            lcd_set_cs0(0);
            lcd_num = 0;
        }
        break;

        case 1: {
            lcd_set_cs0(1);
            lcd_set_cs1(0);
            lcd_num = 1;
        }
        break;
    }
    xSemaphoreGive(lcd_write_mux);
}

static void lcd_write_cmd(uint8_t data)
{
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    lcd_dc_state = 0;
    spi_write_data(&data, 1);
    xSemaphoreGive(lcd_write_mux);
}

static void lcd_write_byte(uint8_t data)
{
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    lcd_dc_state = 1;
    spi_write_data(&data, 1);
    xSemaphoreGive(lcd_write_mux);
}

void lcd_write_data(uint16_t *data, size_t len)
{
    if (len <= 0) {
        return;
    }
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    lcd_dc_state = 1;
    spi_write_data(data, len);
    xSemaphoreGive(lcd_write_mux);
}

void lcd_rst()
{
    lcd_set_res(0);
    lcd_delay_ms(100);
    lcd_set_res(1);
    lcd_delay_ms(100);
}

static void lcd_st7789_config()
{
    lcd_select(0);

    lcd_write_cmd(0x36); // MADCTL (36h): Memory Data Access Control
    lcd_write_byte(0x00);

    lcd_write_cmd(0x3A);  // COLMOD (3Ah): Interface Pixel Format 
    lcd_write_byte(0x05);

    lcd_write_cmd(0xB2); // PORCTRL (B2h): Porch Setting 
    lcd_write_byte(0x0C);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x00);
    lcd_write_byte(0x33);
    lcd_write_byte(0x33); 

    lcd_write_cmd(0xB7); // GCTRL (B7h): Gate Control 
    lcd_write_byte(0x35);  

    lcd_write_cmd(0xBB); // VCOMS (BBh): VCOM Setting 
    lcd_write_byte(0x19);

    lcd_write_cmd(0xC0); // LCMCTRL (C0h): LCM Control 
    lcd_write_byte(0x2C);

    lcd_write_cmd(0xC2); // VDVVRHEN (C2h): VDV and VRH Command Enable
    lcd_write_byte(0x01);

    lcd_write_cmd(0xC3); // VRHS (C3h): VRH Set
    lcd_write_byte(0x12);   

    lcd_write_cmd(0xC4); // VDVS (C4h): VDV Set 
    lcd_write_byte(0x20);  

    lcd_write_cmd(0xC6); // FRCTRL2 (C6h): Frame Rate Control in Normal Mode 
    lcd_write_byte(0x0F);    

    lcd_write_cmd(0xD0); // PWCTRL1 (D0h): Power Control 1 
    lcd_write_byte(0xA4);
    lcd_write_byte(0xA1);

    lcd_write_cmd(0xE0); // PVGAMCTRL (E0h): Positive Voltage Gamma Control
    lcd_write_byte(0xD0);
    lcd_write_byte(0x04);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x11);
    lcd_write_byte(0x13);
    lcd_write_byte(0x2B);
    lcd_write_byte(0x3F);
    lcd_write_byte(0x54);
    lcd_write_byte(0x4C);
    lcd_write_byte(0x18);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x0B);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x23);

    lcd_write_cmd(0xE1); // NVGAMCTRL (E1h): Negative Voltage Gamma Control
    lcd_write_byte(0xD0);
    lcd_write_byte(0x04);
    lcd_write_byte(0x0C);
    lcd_write_byte(0x11);
    lcd_write_byte(0x13);
    lcd_write_byte(0x2C);
    lcd_write_byte(0x3F);
    lcd_write_byte(0x44);
    lcd_write_byte(0x51);
    lcd_write_byte(0x2F);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x1F);
    lcd_write_byte(0x20);
    lcd_write_byte(0x23);

    lcd_write_cmd(0x21); // INVON (21h): Display Inversion On

    lcd_write_cmd(0x11); // SLPOUT (11h): Sleep Out 

    lcd_write_cmd(0x29); // DISPON (29h): Display On
}

static void lcd_st7735_config()
{
    lcd_select(1);
    lcd_write_cmd(0x11);//Sleep exit
    // lcd_delay_ms(120);
    lcd_write_cmd(0x21);

    lcd_write_cmd(0xB1);
    lcd_write_byte(0x05);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x3A);

    lcd_write_cmd(0xB2);
    lcd_write_byte(0x05);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x3A);

    lcd_write_cmd(0xB3);
    lcd_write_byte(0x05);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x05);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x3A);

    lcd_write_cmd(0xB4);
    lcd_write_byte(0x03);

    lcd_write_cmd(0xC0);
    lcd_write_byte(0x62);
    lcd_write_byte(0x02);
    lcd_write_byte(0x04);

    lcd_write_cmd(0xC1);
    lcd_write_byte(0xC0);

    lcd_write_cmd(0xC2);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x00);

    lcd_write_cmd(0xC3);
    lcd_write_byte(0x8D);
    lcd_write_byte(0x6A);

    lcd_write_cmd(0xC4);
    lcd_write_byte(0x8D);
    lcd_write_byte(0xEE);

    lcd_write_cmd(0xC5);  /*VCOM*/
    lcd_write_byte(0x0E);

    lcd_write_cmd(0xE0);
    lcd_write_byte(0x10);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x02);
    lcd_write_byte(0x03);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x07);
    lcd_write_byte(0x02);
    lcd_write_byte(0x07);
    lcd_write_byte(0x0A);
    lcd_write_byte(0x12);
    lcd_write_byte(0x27);
    lcd_write_byte(0x37);
    lcd_write_byte(0x00);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x10);

    lcd_write_cmd(0xE1);
    lcd_write_byte(0x10);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x03);
    lcd_write_byte(0x03);
    lcd_write_byte(0x0F);
    lcd_write_byte(0x06);
    lcd_write_byte(0x02);
    lcd_write_byte(0x08);
    lcd_write_byte(0x0A);
    lcd_write_byte(0x13);
    lcd_write_byte(0x26);
    lcd_write_byte(0x36);
    lcd_write_byte(0x00);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x10);

    lcd_write_cmd(0x3A);
    lcd_write_byte(0x05);

    lcd_write_cmd(0x36); // Memory Data Access Control
    lcd_write_byte(0xA8); // C8/A8/08/68

    lcd_write_cmd(0x29);
}

void lcd_init()
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = LCD_PIN_MOSI,
        .sclk_io_num = LCD_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SPI_BURST_MAX_LEN
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40*1000*1000,           //Clock out at 10 MHz
        .mode = 0,                                //SPI mode 0
        .spics_io_num = -1,                       //CS pin
        .queue_size = 1,                          //We want to be able to queue 1 transactions at a time
        .pre_cb = spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
        .flags = SPI_DEVICE_HALFDUPLEX
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    //Initialize non-SPI GPIOs
    gpio_set_direction(LCD_PIN_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_PIN_CS0, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_PIN_CS1, GPIO_MODE_OUTPUT);
    // gpio_set_direction(LCD_PIN_BCKL, GPIO_MODE_OUTPUT);
    lcd_set_cs0(1);
    lcd_set_cs1(1);
    
    lcd_write_mux = xSemaphoreCreateMutex();

    lcd_rst();//lcd_rst before LCD Init.
    lcd_delay_ms(100);
    lcd_st7789_config();
    lcd_st7735_config();

    lcd_set_blk(1);
    printf("lcd init ok\n");
}

void lcd_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    switch (lcd_num) {
        case 0: {
            lcd_write_cmd(0x2a);    // CASET (2Ah): Column Address Set 
            // Must write byte than byte
            lcd_write_byte(0x00);
            lcd_write_byte(x_start);
            lcd_write_byte(0x00);
            lcd_write_byte(x_end);

            lcd_write_cmd(0x2b);    // RASET (2Bh): Row Address Set 
            lcd_write_byte(0x00);
            lcd_write_byte(y_start);
            lcd_write_byte(0x00);
            lcd_write_byte(y_end);    
            lcd_write_cmd(0x2c);    // RAMWR (2Ch): Memory Write 
        }
        break;

        case 1: {
    lcd_write_cmd(0x2a);    // CASET (2Ah): Column Address Set
    // Must write byte than byte
    lcd_write_byte(0x00);
    lcd_write_byte(x_start + 1);
    lcd_write_byte(0x00);
    lcd_write_byte(x_end + 1);

    lcd_write_cmd(0x2b);    // RASET (2Bh): Row Address Set
    lcd_write_byte(0x00);
    lcd_write_byte(y_start + 0x1A);
    lcd_write_byte(0x00);
    lcd_write_byte(y_end + 0x1A);
    lcd_write_cmd(0x2c);    // RAMWR (2Ch): Memory Write
        }
        break;
    }

}