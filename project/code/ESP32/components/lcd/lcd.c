
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "lcd.h"

static spi_device_handle_t spi;
static SemaphoreHandle_t lcd_write_mux = NULL;

static void lcd_delay_ms(uint32_t time)
{
    vTaskDelay(time / portTICK_RATE_MS);
}

void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    int dc = (int)t->user;
    lcd_set_dc(dc);
}

static void lcd_write_cmd(uint8_t data)
{
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));         //Zero out the transaction
    t.length = 8;                     //Command is 8 bits
    t.tx_buffer = &data;              //The data is the cmd itself
    t.user = (void*)0;                //D/C needs to be set to 0
    spi_device_polling_transmit(spi, &t);  //Transmit!
    xSemaphoreGive(lcd_write_mux);
}

static void lcd_write_byte(uint8_t data)
{
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));         //Zero out the transaction
    t.length = 8;                     //data is 8 bits
    t.tx_buffer = &data;              //The data is the buf itself
    t.user = (void*)1;                //D/C needs to be set to 1
    spi_device_polling_transmit(spi, &t);  //Transmit!
    xSemaphoreGive(lcd_write_mux);
}

void lcd_write_data(uint16_t *data, int len)
{
    if (len <= 0) {
        return;
    }
    xSemaphoreTake(lcd_write_mux, portMAX_DELAY);
    spi_transaction_t t, *rtrans;
    memset(&t, 0, sizeof(t));         //Zero out the transaction
    t.length = 16 * len;               
    t.tx_buffer = data;
    t.user = (void*)1;                //D/C needs to be set to 1
    spi_device_queue_trans(spi, &t, portMAX_DELAY);
    spi_device_get_trans_result(spi, &rtrans, portMAX_DELAY);
    xSemaphoreGive(lcd_write_mux);
}

void lcd_rst()
{
    lcd_set_res(0);
    lcd_delay_ms(100);
    lcd_set_res(1);
    lcd_delay_ms(100);
}

static void lcd_st7789v_config()
{
    /* Memory Data Access Control, MX=MV=1, MY=ML=MH=0, RGB=0 */
    lcd_write_cmd(0x36);
    lcd_write_byte((1<<5)|(1<<6));
    /* Interface Pixel Format, 16bits/pixel for RGB/MCU interface */
    lcd_write_cmd(0x3A);
    lcd_write_byte(0x55);
    /* Porch Setting */
    lcd_write_cmd(0xB2);
    lcd_write_byte(0x0c);
    lcd_write_byte(0x0c);
    lcd_write_byte(0x00);
    lcd_write_byte(0x33);
    lcd_write_byte(0x33);
    /* Gate Control, Vgh=13.65V, Vgl=-10.43V */
    lcd_write_cmd(0xB7);
    lcd_write_byte(0x45);
    /* VCOM Setting, VCOM=1.175V */
    lcd_write_cmd(0xBB);
    lcd_write_byte(0x2B);
    /* LCM Control, XOR: BGR, MX, MH */
    lcd_write_cmd(0xC0);
    lcd_write_byte(0x2C);
    /* VDV and VRH Command Enable, enable=1 */
    lcd_write_cmd(0xC2);
    lcd_write_byte(0x01);
    lcd_write_byte(0xff);
    /* VRH Set, Vap=4.4+... */
    lcd_write_cmd(0xC3);
    lcd_write_byte(0x11);
    /* VDV Set, VDV=0 */
    lcd_write_cmd(0xC4);
    lcd_write_byte(0x20);
    /* Frame Rate Control, 60Hz, inversion=0 */
    lcd_write_cmd(0xC6);
    lcd_write_byte(0x0f);
    /* Power Control 1, AVDD=6.8V, AVCL=-4.8V, VDDS=2.3V */
    lcd_write_cmd(0xD0);
    lcd_write_byte(0xA4);
    // lcd_write_byte(0xA1);
    /* Positive Voltage Gamma Control */
    lcd_write_cmd(0xE0);
    lcd_write_byte(0xD0);
    lcd_write_byte(0x00);
    lcd_write_byte(0x05);
    lcd_write_byte(0x0E);
    lcd_write_byte(0x15);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x37);
    lcd_write_byte(0x43);
    lcd_write_byte(0x47);
    lcd_write_byte(0x09);
    lcd_write_byte(0x15);
    lcd_write_byte(0x12);
    lcd_write_byte(0x16);
    lcd_write_byte(0x19);
    /* Negative Voltage Gamma Control */
    lcd_write_cmd(0xE1);
    lcd_write_byte(0xD0); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x05); 
    lcd_write_byte(0x0D); 
    lcd_write_byte(0x0C); 
    lcd_write_byte(0x06); 
    lcd_write_byte(0x2D); 
    lcd_write_byte(0x44); 
    lcd_write_byte(0x40); 
    lcd_write_byte(0x0E); 
    lcd_write_byte(0x1C); 
    lcd_write_byte(0x18); 
    lcd_write_byte(0x16); 
    lcd_write_byte(0x19);
    /* Sleep Out */
    lcd_write_cmd(0x11);
    lcd_delay_ms(100);
    /* Display On */
    lcd_write_cmd(0x29);
    lcd_delay_ms(100);
}

static void lcd_ili9341_config()
{
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    lcd_write_cmd(0xCF); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x83); 
    lcd_write_byte(0X30);
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    lcd_write_cmd(0xED); 
    lcd_write_byte(0x64); 
    lcd_write_byte(0x03); 
    lcd_write_byte(0X12); 
    lcd_write_byte(0X81);
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    lcd_write_cmd(0xE8); 
    lcd_write_byte(0x85); 
    lcd_write_byte(0x01); 
    lcd_write_byte(0x79);
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    lcd_write_cmd(0xCB); 
    lcd_write_byte(0x39); 
    lcd_write_byte(0x2C); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x34); 
    lcd_write_byte(0x02);
    /* Pump ratio control, DDVDH=2xVCl */
    lcd_write_cmd(0xF7); 
    lcd_write_byte(0x20);
    /* Driver timing control, all=0 unit */
    lcd_write_cmd(0xEA); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x00);
    /* Power control 1, GVDD=4.75V */
    lcd_write_cmd(0xC0); 
    lcd_write_byte(0x26);
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    lcd_write_cmd(0xC1); 
    lcd_write_byte(0x11);
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    lcd_write_cmd(0xC5); 
    lcd_write_byte(0x35); 
    lcd_write_byte(0x3E);
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    lcd_write_cmd(0xC7); 
    lcd_write_byte(0xBE);
    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
    lcd_write_cmd(0x36); 
    lcd_write_byte(0x28);
    /* Pixel format, 16bits/pixel for RGB/MCU interface */
    lcd_write_cmd(0x3A); 
    lcd_write_byte(0x55);
    /* Frame rate control, f=fosc, 70Hz fps */
    lcd_write_cmd(0xB1); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x1B);
    /* Enable 3G, disabled */
    lcd_write_cmd(0xF2); 
    lcd_write_byte(0x08);
    /* Gamma set, curve 1 */
    lcd_write_cmd(0x26); 
    lcd_write_byte(0x01);
    /* Positive gamma correction */
    lcd_write_cmd(0xE0); 
    lcd_write_byte(0x1F);
    lcd_write_byte(0x1A);
    lcd_write_byte(0x18);
    lcd_write_byte(0x0A);
    lcd_write_byte(0x0F);
    lcd_write_byte(0x06);
    lcd_write_byte(0x45);
    lcd_write_byte(0X87);
    lcd_write_byte(0x32);
    lcd_write_byte(0x0A);
    lcd_write_byte(0x07);
    lcd_write_byte(0x02);
    lcd_write_byte(0x07);
    lcd_write_byte(0x05);
    lcd_write_byte(0x00);
    /* Negative gamma correction */
    lcd_write_cmd(0XE1); 
    lcd_write_byte(0x00);
    lcd_write_byte(0x25);
    lcd_write_byte(0x27);
    lcd_write_byte(0x05);
    lcd_write_byte(0x10);
    lcd_write_byte(0x09);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x78);
    lcd_write_byte(0x4D);
    lcd_write_byte(0x05);
    lcd_write_byte(0x18);
    lcd_write_byte(0x0D);
    lcd_write_byte(0x38);
    lcd_write_byte(0x3A);
    lcd_write_byte(0x1F);
    /* Column address set, SC=0, EC=0xEF */
    lcd_write_cmd(0x2A); 
    lcd_write_byte(0x00);
    lcd_write_byte(0x00);
    lcd_write_byte(0x00);
    lcd_write_byte(0xEF);
    /* Page address set, SP=0, EP=0x013F */
    lcd_write_cmd(0x2B); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x00); 
    lcd_write_byte(0x01); 
    lcd_write_byte(0x3f);
    /* Memory write */
    lcd_write_cmd(0x2C); 
    // lcd_write_byte(0);
    /* Entry mode set, Low vol detect disabled, normal display */
    lcd_write_cmd(0xB7); 
    lcd_write_byte(0x07);
    /* Display function control */
    lcd_write_cmd(0xB6); 
    lcd_write_byte(0x0A);
    lcd_write_byte(0x82);
    lcd_write_byte(0x27);
    lcd_write_byte(0x00);
    /* Sleep out */
    lcd_write_cmd(0x11); 
    lcd_delay_ms(100);
    /* Display on */
    lcd_write_cmd(0x29); 
    lcd_delay_ms(100);
}

void lcd_init()
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = LCD_PIN_MISO,
        .mosi_io_num = LCD_PIN_MOSI,
        .sclk_io_num = LCD_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_BURST_MAX_LEN * 2
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20*1000*1000,           //Clock out at 10 MHz
        .mode = 0,                                //SPI mode 0
        .spics_io_num = LCD_PIN_CS,               //CS pin
        .queue_size = 1,                          //We want to be able to queue 1 transactions at a time
        .pre_cb = lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
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
    gpio_set_direction(LCD_PIN_BCKL, GPIO_MODE_OUTPUT);
    
    lcd_write_mux = xSemaphoreCreateMutex();

    lcd_rst();//lcd_rst before LCD Init.

    lcd_ili9341_config();

    lcd_set_blk(1);
    printf("lcd init ok\n");
}

void lcd_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
    lcd_write_cmd(0x2a);    // CASET (2Ah): Column Address Set
    // Must write byte than byte
    lcd_write_byte((x_start) >> 8);
    lcd_write_byte((x_start) & 0xff);
    lcd_write_byte((x_end) >> 8);
    lcd_write_byte((x_end) & 0xff);

    lcd_write_cmd(0x2b);    // RASET (2Bh): Row Address Set
    lcd_write_byte((y_start) >> 8);
    lcd_write_byte((y_start) & 0xff);
    lcd_write_byte((y_end) >> 8);
    lcd_write_byte((y_end) & 0xff);
    lcd_write_cmd(0x2c);    // RAMWR (2Ch): Memory Write
}