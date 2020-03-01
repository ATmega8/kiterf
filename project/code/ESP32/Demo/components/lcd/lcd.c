
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

static void lcd_hx8357_write_reg(uint8_t index, uint8_t data)
{
    lcd_write_cmd(index);
    lcd_write_byte(data);
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
    lcd_delay_ms(1000);
    lcd_set_res(1);
    lcd_delay_ms(1000);
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

static void lcd_hx8357_set_index(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{
    //HX8357-A
    lcd_hx8357_write_reg(0x80,Ystart>>8); // Set CAC=0x0000
    lcd_hx8357_write_reg(0x81,Ystart);
    lcd_hx8357_write_reg(0x82,Xstart>>8); // Set RAC=0x0000
    lcd_hx8357_write_reg(0x83,Xstart);

    lcd_hx8357_write_reg(0x02,Xstart>>8);
    lcd_hx8357_write_reg(0x03,Xstart);     //Column Start
    lcd_hx8357_write_reg(0x04,Xend>>8);
    lcd_hx8357_write_reg(0x05,Xend);     //Column End

    lcd_hx8357_write_reg(0x06,Ystart>>8);
    lcd_hx8357_write_reg(0x07,Ystart);     //Row Start
    lcd_hx8357_write_reg(0x08,Yend>>8);
    lcd_hx8357_write_reg(0x09,Yend);     //Row End

    lcd_write_cmd(0x22);
}

static void lcd_hx8357_config()
{
    // LCD_FSMC_Config();
    // LCD_Rst();
    //================= Command page 0 =================
    lcd_hx8357_write_reg(0xFF,0x00); // Command page 0
    lcd_delay_ms(15);
    lcd_hx8357_write_reg(0x1A,0x04);//VGH VGL VCL  DDVDH
    lcd_hx8357_write_reg(0x1B,0x1C);
    // Power Settng
    lcd_hx8357_write_reg(0x23,0x94); // Set VCOM offset, VMF=0x52
    lcd_hx8357_write_reg(0x24,0x69); // Set VCOMH voltage, VHH=0x64
    lcd_hx8357_write_reg(0x25,0x63); // Set VCOML voltage, VML=0x71
    lcd_hx8357_write_reg(0x19,0x01);
    lcd_delay_ms(10);
    lcd_hx8357_write_reg(0x1A,0x00);//VGH VGL VCL  DDVDH
    lcd_hx8357_write_reg(0x1F,0x8A);//
    lcd_hx8357_write_reg(0x01,0x00);//
    lcd_hx8357_write_reg(0x1C,0x05);//
    lcd_hx8357_write_reg(0x1F,0x82);//
    lcd_delay_ms(10);

    lcd_hx8357_write_reg(0x1F,0x92);//
    lcd_delay_ms(10);
    lcd_hx8357_write_reg(0x1F,0xD4);//
    // Set GRAM aea 320x480
    lcd_hx8357_write_reg(0x02,0x00);
    lcd_hx8357_write_reg(0x03,0x00);
    lcd_hx8357_write_reg(0x04,0x01);
    lcd_hx8357_write_reg(0x05,0x3F);
    lcd_hx8357_write_reg(0x06,0x00);
    lcd_hx8357_write_reg(0x07,0x00);
    lcd_hx8357_write_reg(0x08,0x01);
    lcd_hx8357_write_reg(0x09,0xDF);

    lcd_hx8357_write_reg(0x16,0x88);//set my mx mv bgr...
    lcd_hx8357_write_reg(0x17,0x55);//262-bit/pixel 0X60 ;262-bit/pixel 0X50
    lcd_hx8357_write_reg(0x18,0x21);	//Fosc=130%*5.2MHZ   21
    lcd_hx8357_write_reg(0x1D,0x00); // FS0[1:0]=01, Set the operating frequency of the step-up circuit 1
    lcd_hx8357_write_reg(0x1E,0x00);

    lcd_hx8357_write_reg(0x26,0x33);
    lcd_hx8357_write_reg(0x27,0x01);
    lcd_hx8357_write_reg(0x29,0x00);
    lcd_hx8357_write_reg(0x2A,0x00);
    lcd_hx8357_write_reg(0x2B,0x01);//0A
    lcd_hx8357_write_reg(0x2C,0x0A);
    lcd_hx8357_write_reg(0x2D,0x20);
    lcd_hx8357_write_reg(0x2E,0xA3);
    lcd_hx8357_write_reg(0x2F,0x00);//00 100416

    lcd_hx8357_write_reg(0x31,0x00);//RGB MODE1  0X03=RGB MODE2
    lcd_hx8357_write_reg(0x32,0x00);
    lcd_hx8357_write_reg(0x33,0x08);
    lcd_hx8357_write_reg(0x34,0x08);
    lcd_hx8357_write_reg(0x36,0x02);//REV

    // Gamma
    lcd_hx8357_write_reg(0x40,0x01);
    lcd_hx8357_write_reg(0x41,0x0F);
    lcd_hx8357_write_reg(0x42,0x0F);
    lcd_hx8357_write_reg(0x43,0x26);
    lcd_hx8357_write_reg(0x44,0x2C);
    lcd_hx8357_write_reg(0x45,0x3C);
    lcd_hx8357_write_reg(0x46,0x0B);
    lcd_hx8357_write_reg(0x47,0x5F);
    lcd_hx8357_write_reg(0x48,0x00);
    lcd_hx8357_write_reg(0x49,0x06);
    lcd_hx8357_write_reg(0x4A,0x09);
    lcd_hx8357_write_reg(0x4B,0x0E);
    lcd_hx8357_write_reg(0x4C,0x16);

    lcd_hx8357_write_reg(0x50,0x01);
    lcd_hx8357_write_reg(0x51,0x1D);
    lcd_hx8357_write_reg(0x52,0x21);
    lcd_hx8357_write_reg(0x53,0x3A);
    lcd_hx8357_write_reg(0x54,0x39);
    lcd_hx8357_write_reg(0x55,0x3F);
    lcd_hx8357_write_reg(0x56,0x2D);
    lcd_hx8357_write_reg(0x57,0x7F);
    lcd_hx8357_write_reg(0x58,0x02);
    lcd_hx8357_write_reg(0x59,0x15);
    lcd_hx8357_write_reg(0x5A,0x1B);
    lcd_hx8357_write_reg(0x5B,0x1B);
    lcd_hx8357_write_reg(0x5C,0x1A);
    lcd_hx8357_write_reg(0x5D,0x55);
//================= Command page 1 =================
    lcd_hx8357_write_reg(0xff,0x01);
    lcd_hx8357_write_reg(0x00,0x01);
    lcd_hx8357_write_reg(0x01,0x00);
    lcd_hx8357_write_reg(0x02,0x01);
    lcd_hx8357_write_reg(0x03,0x03);
    lcd_hx8357_write_reg(0x04,0x05);
    lcd_hx8357_write_reg(0x05,0x06);
    lcd_hx8357_write_reg(0x06,0x08);
    lcd_hx8357_write_reg(0x07,0x0C);
    lcd_hx8357_write_reg(0x08,0x0E);
    lcd_hx8357_write_reg(0x09,0x11);
    lcd_hx8357_write_reg(0x0A,0x12);
    lcd_hx8357_write_reg(0x0B,0x14);
    lcd_hx8357_write_reg(0x0C,0x1B);
    lcd_hx8357_write_reg(0x0D,0x23);
    lcd_hx8357_write_reg(0x0E,0x29);
    lcd_hx8357_write_reg(0x0F,0x2F);
    lcd_hx8357_write_reg(0x10,0x34);
    lcd_hx8357_write_reg(0x11,0x39);
    lcd_hx8357_write_reg(0x12,0x3E);
    lcd_hx8357_write_reg(0x13,0x43);
    lcd_hx8357_write_reg(0x14,0x48);
    lcd_hx8357_write_reg(0x15,0x4C);
    lcd_hx8357_write_reg(0x16,0x51);
    lcd_hx8357_write_reg(0x17,0x55);
    lcd_hx8357_write_reg(0x18,0x59);
    lcd_hx8357_write_reg(0x19,0x5D);
    lcd_hx8357_write_reg(0x1A,0x60);
    lcd_hx8357_write_reg(0x1B,0x64);
    lcd_hx8357_write_reg(0x1C,0x68);
    lcd_hx8357_write_reg(0x1D,0x6C);
    lcd_hx8357_write_reg(0x1E,0x70);
    lcd_hx8357_write_reg(0x1F,0x73);
    lcd_hx8357_write_reg(0x20,0x77);
    lcd_hx8357_write_reg(0x21,0x7B);
    lcd_hx8357_write_reg(0x22,0x7F);
    lcd_hx8357_write_reg(0x23,0x83);
    lcd_hx8357_write_reg(0x24,0x87);
    lcd_hx8357_write_reg(0x25,0x8A);
    lcd_hx8357_write_reg(0x26,0x8E);
    lcd_hx8357_write_reg(0x27,0x92);
    lcd_hx8357_write_reg(0x28,0x96);
    lcd_hx8357_write_reg(0x29,0x9A);
    lcd_hx8357_write_reg(0x2A,0x9F);
    lcd_hx8357_write_reg(0x2B,0xA3);
    lcd_hx8357_write_reg(0x2C,0xA7);
    lcd_hx8357_write_reg(0x2D,0xAC);
    lcd_hx8357_write_reg(0x2E,0xAF);
    lcd_hx8357_write_reg(0x2F,0xB3);
    lcd_hx8357_write_reg(0x30,0xB7);
    lcd_hx8357_write_reg(0x31,0xBA);
    lcd_hx8357_write_reg(0x32,0xBE);
    lcd_hx8357_write_reg(0x33,0xC3);
    lcd_hx8357_write_reg(0x34,0xC7);
    lcd_hx8357_write_reg(0x35,0xCC);
    lcd_hx8357_write_reg(0x36,0xD1);
    lcd_hx8357_write_reg(0x37,0xD7);
    lcd_hx8357_write_reg(0x38,0xDD);
    lcd_hx8357_write_reg(0x39,0xE3);
    lcd_hx8357_write_reg(0x3A,0xE8);
    lcd_hx8357_write_reg(0x3B,0xEA);
    lcd_hx8357_write_reg(0x3C,0xEC);
    lcd_hx8357_write_reg(0x3D,0xEF);
    lcd_hx8357_write_reg(0x3E,0xF3);
    lcd_hx8357_write_reg(0x3F,0xF6);

    lcd_hx8357_write_reg(0x40,0xFE);
    lcd_hx8357_write_reg(0x41,0x00);
    lcd_hx8357_write_reg(0x42,0x01);
    lcd_hx8357_write_reg(0x43,0x03);
    lcd_hx8357_write_reg(0x44,0x05);
    lcd_hx8357_write_reg(0x45,0x06);
    lcd_hx8357_write_reg(0x46,0x08);
    lcd_hx8357_write_reg(0x47,0x0C);
    lcd_hx8357_write_reg(0x48,0x0E);
    lcd_hx8357_write_reg(0x49,0x11);
    lcd_hx8357_write_reg(0x4A,0x12);
    lcd_hx8357_write_reg(0x4B,0x14);
    lcd_hx8357_write_reg(0x4C,0x1B);
    lcd_hx8357_write_reg(0x4D,0x23);
    lcd_hx8357_write_reg(0x4E,0x29);
    lcd_hx8357_write_reg(0x4F,0x2F);
    lcd_hx8357_write_reg(0x50,0x34);
    lcd_hx8357_write_reg(0x51,0x39);
    lcd_hx8357_write_reg(0x52,0x3E);
    lcd_hx8357_write_reg(0x53,0x43);
    lcd_hx8357_write_reg(0x54,0x48);
    lcd_hx8357_write_reg(0x55,0x4C);
    lcd_hx8357_write_reg(0x56,0x51);
    lcd_hx8357_write_reg(0x57,0x55);
    lcd_hx8357_write_reg(0x58,0x59);
    lcd_hx8357_write_reg(0x59,0x5D);
    lcd_hx8357_write_reg(0x5A,0x60);
    lcd_hx8357_write_reg(0x5B,0x64);
    lcd_hx8357_write_reg(0x5C,0x68);
    lcd_hx8357_write_reg(0x5D,0x6C);

    lcd_hx8357_write_reg(0x5E,0x70);
    lcd_hx8357_write_reg(0x5F,0x73);
    lcd_hx8357_write_reg(0x60,0x77);
    lcd_hx8357_write_reg(0x61,0x7B);
    lcd_hx8357_write_reg(0x62,0x7F);
    lcd_hx8357_write_reg(0x63,0x83);
    lcd_hx8357_write_reg(0x64,0x87);
    lcd_hx8357_write_reg(0x65,0x8A);
    lcd_hx8357_write_reg(0x66,0x8E);
    lcd_hx8357_write_reg(0x67,0x92);
    lcd_hx8357_write_reg(0x68,0x96);
    lcd_hx8357_write_reg(0x69,0x9A);
    lcd_hx8357_write_reg(0x6A,0x9F);
    lcd_hx8357_write_reg(0x6B,0xA3);
    lcd_hx8357_write_reg(0x6C,0xA7);
    lcd_hx8357_write_reg(0x6D,0xAC);
    lcd_hx8357_write_reg(0x6E,0xAF);
    lcd_hx8357_write_reg(0x6F,0xB3);
    lcd_hx8357_write_reg(0x70,0xB7);
    lcd_hx8357_write_reg(0x71,0xBA);
    lcd_hx8357_write_reg(0x72,0xBE);
    lcd_hx8357_write_reg(0x73,0xC3);
    lcd_hx8357_write_reg(0x74,0xC7);
    lcd_hx8357_write_reg(0x75,0xCC);
    lcd_hx8357_write_reg(0x76,0xD1);
    lcd_hx8357_write_reg(0x77,0xD7);
    lcd_hx8357_write_reg(0x78,0xDD);
    lcd_hx8357_write_reg(0x79,0xE3);
    lcd_hx8357_write_reg(0x7A,0xE8);
    lcd_hx8357_write_reg(0x7B,0xEA);
    lcd_hx8357_write_reg(0x7C,0xEC);
    lcd_hx8357_write_reg(0x7D,0xEF);
    lcd_hx8357_write_reg(0x7E,0xF3);
    lcd_hx8357_write_reg(0x7F,0xF6);
    lcd_hx8357_write_reg(0x80,0xFE);
    lcd_hx8357_write_reg(0x81,0x00);
    lcd_hx8357_write_reg(0x82,0x01);
    lcd_hx8357_write_reg(0x83,0x03);
    lcd_hx8357_write_reg(0x84,0x05);
    lcd_hx8357_write_reg(0x85,0x06);
    lcd_hx8357_write_reg(0x86,0x08);
    lcd_hx8357_write_reg(0x87,0x0C);
    lcd_hx8357_write_reg(0x88,0x0E);
    lcd_hx8357_write_reg(0x89,0x11);
    lcd_hx8357_write_reg(0x8A,0x12);
    lcd_hx8357_write_reg(0x8B,0x14);
    lcd_hx8357_write_reg(0x8C,0x1B);
    lcd_hx8357_write_reg(0x8D,0x23);
    lcd_hx8357_write_reg(0x8E,0x29);
    lcd_hx8357_write_reg(0x8F,0x2F);
    lcd_hx8357_write_reg(0x90,0x34);
    lcd_hx8357_write_reg(0x91,0x39);
    lcd_hx8357_write_reg(0x92,0x3E);
    lcd_hx8357_write_reg(0x93,0x43);
    lcd_hx8357_write_reg(0x94,0x48);
    lcd_hx8357_write_reg(0x95,0x4C);
    lcd_hx8357_write_reg(0x96,0x51);
    lcd_hx8357_write_reg(0x97,0x55);
    lcd_hx8357_write_reg(0x98,0x59);
    lcd_hx8357_write_reg(0x99,0x5D);
    lcd_hx8357_write_reg(0x9A,0x60);
    lcd_hx8357_write_reg(0x9B,0x64);
    lcd_hx8357_write_reg(0x9C,0x68);
    lcd_hx8357_write_reg(0x9D,0x6C);
    lcd_hx8357_write_reg(0x9E,0x70);
    lcd_hx8357_write_reg(0x9F,0x73);
    lcd_hx8357_write_reg(0xA0,0x77);
    lcd_hx8357_write_reg(0xA1,0x7B);
    lcd_hx8357_write_reg(0xA2,0x7F);
    lcd_hx8357_write_reg(0xA3,0x83);
    lcd_hx8357_write_reg(0xA4,0x87);
    lcd_hx8357_write_reg(0xA5,0x8A);
    lcd_hx8357_write_reg(0xA6,0x8E);
    lcd_hx8357_write_reg(0xA7,0x92);
    lcd_hx8357_write_reg(0xA8,0x96);
    lcd_hx8357_write_reg(0xA9,0x9A);
    lcd_hx8357_write_reg(0xAA,0x9F);
    lcd_hx8357_write_reg(0xAB,0xA3);
    lcd_hx8357_write_reg(0xAC,0xA7);
    lcd_hx8357_write_reg(0xAD,0xAC);
    lcd_hx8357_write_reg(0xAE,0xAF);
    lcd_hx8357_write_reg(0xAF,0xB3);
    lcd_hx8357_write_reg(0xB0,0xB7);
    lcd_hx8357_write_reg(0xB1,0xBA);
    lcd_hx8357_write_reg(0xB2,0xBE);
    lcd_hx8357_write_reg(0xB3,0xC3);
    lcd_hx8357_write_reg(0xB4,0xC7);
    lcd_hx8357_write_reg(0xB5,0xCC);
    lcd_hx8357_write_reg(0xB6,0xD1);
    lcd_hx8357_write_reg(0xB7,0xD7);
    lcd_hx8357_write_reg(0xB8,0xDD);
    lcd_hx8357_write_reg(0xB9,0xE3);
    lcd_hx8357_write_reg(0xBA,0xE8);
    lcd_hx8357_write_reg(0xBB,0xEA);
    lcd_hx8357_write_reg(0xBC,0xEC);
    lcd_hx8357_write_reg(0xBD,0xEF);
    lcd_hx8357_write_reg(0xBE,0xF3);
    lcd_hx8357_write_reg(0xBF,0xF6);
    lcd_hx8357_write_reg(0xC0,0xFE);

//================= Command page 0 =================
    lcd_hx8357_write_reg(0xff,0x00);
    lcd_hx8357_write_reg(0x60,0x08);
    lcd_hx8357_write_reg(0xF2,0x00);
    lcd_hx8357_write_reg(0xE4,0x1F); //EQVCI_M1=0x00
    lcd_hx8357_write_reg(0xE5,0x1F); //EQGND_M1=0x1C
    lcd_hx8357_write_reg(0xE6,0x20); //EQVCI_M0=0x1C
    lcd_hx8357_write_reg(0xE7,0x00); //EQGND_M0=0x1C
    lcd_hx8357_write_reg(0xE8,0xD1);
    lcd_hx8357_write_reg(0xE9,0xC0);
    lcd_hx8357_write_reg(0x28,0x38);
    lcd_delay_ms(80);
    lcd_hx8357_write_reg(0x28,0x3C);// GON=0, DTE=0, D[1:0]=01

    lcd_hx8357_write_reg(0x80,0x00);
    lcd_hx8357_write_reg(0x81,0x00);
    lcd_hx8357_write_reg(0x82,0x00);
    lcd_hx8357_write_reg(0x83,0x00);

    lcd_delay_ms(100);
    lcd_hx8357_write_reg(0x16,0x28);

    lcd_write_cmd(0x22);
    lcd_delay_ms(120);
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

# if defined(LCD_ILI9341)
    lcd_ili9341_config();
#elif defined(LCD_HX8357)
    lcd_hx8357_config();
#elif defined(LCD_ST7789)
    lcd_st7789v_config();
#else
    #error "please define lcd type(LCD_ILI9341,LCD_HX8357,LCD_ST7789)"
#endif

    lcd_set_blk(1);
    printf("lcd init ok\n");
}


static void lcd_ili9341_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
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

void lcd_set_index(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end)
{
# if defined(LCD_ILI9341)
    lcd_ili9341_set_index(x_start, y_start, x_end, y_end);
#elif defined(LCD_HX8357)
    lcd_hx8357_set_index(x_start, x_end, y_start, y_end);
#elif defined(LCD_ST7789)
    lcd_ili9341_set_index(x_start, y_start, x_end, y_end);
#else
    #error "please define lcd type(LCD_ILI9341,LCD_HX8357,LCD_ST7789)"
#endif
}