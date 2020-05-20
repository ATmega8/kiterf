//
// Created by life on 19-11-20.
//

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Mirics MSi001 silicon tuner driver
 *
 * Copyright (C) 2013 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2014 Antti Palosaari <crope@iki.fi>
 */

#include "msi001.h"
#include <string.h>

#include "driver/spi_master.h"
#include "esp_log.h"
#include "rfswitch.h"
#include "soft_spi.h"

static const char* MSI001_TAG="MSI001";

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  16
#define PIN_NUM_CS   17

static spi_device_handle_t  spi;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define swap(a, b) \
    do { typeof(a) __tmp = (a); (a) = (b); (b) = __tmp; } while (0)

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))

#define DIV_ROUND_CLOSEST(x, divisor)(            \
{                            \
    typeof(x) __x = x;                \
    typeof(divisor) __d = divisor;            \
    (((typeof(x))-1) > 0 ||                \
     ((typeof(divisor))-1) > 0 ||            \
     (((__x) > 0) == ((__d) > 0))) ?        \
    (((__x) + ((__d) / 2)) / (__d)) :    \
    (((__x) - ((__d) / 2)) / (__d));    \
}                            \
)

static inline uint64_t div_u64_rem(uint64_t dividend, uint32_t divisor, uint32_t *remainder) {
    *remainder = dividend % divisor;
    return dividend / divisor;
}

static inline uint64_t div_u64(uint64_t dividend, uint32_t divisor) {
    uint32_t remainder;
    return div_u64_rem(dividend, divisor, &remainder);
}

static unsigned long gcd(unsigned long a, unsigned long b) {
    unsigned long r = a | b;

    if (!a || !b) {
        return r;
    }

    /* Isolate lsbit of r */
    r &= -r;

    while (!(b & r)) {
        b >>= 1;
    }

    if (b == r) {
        return r;
    }

    for (;;) {
        while (!(a & r)) {
            a >>= 1;
        }

        if (a == r) {
            return r;
        }

        if (a == b) {
            return a;
        }

        if (a < b) {
            swap(a, b);
        }

        a -= b;
        a >>= 1;

        if (a & r) {
            a += b;
        }

        a >>= 1;
    }
}

static int spi_write(uint8_t *pdata, uint8_t len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return 0;             //no need to send anything

    memset(&t, 0, sizeof(t));       //Zero out the transaction

    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=pdata;               //Data
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
    return 0;
}

int msi001_wreg(uint32_t data)
{
    uint8_t buf[3];
    buf[2] = (uint8_t) (data & 0x000000FF);
    buf[1] = (uint8_t) ((data & 0x0000FF00) >> 8);
    buf[0] = (uint8_t) ((data & 0x00FF0000) >> 16);
    /* Register format: 4 bits addr + 20 bits value */
    soft_spi_set_cs(0);
    ets_delay_us(1);
    soft_spi_trans_byte(buf[0]);
    soft_spi_trans_byte(buf[1]);
    soft_spi_trans_byte(buf[2]);
    ets_delay_us(1);
    soft_spi_set_cs(1);
    return 0;
};

int msi001_set_gain(uint32_t gain)
{
    uint32_t reg;

    reg = 1 << 0;
    reg |= gain << 4;
    reg |= 0 << 10;
    reg |= 0 << 12;
    reg |= 0 << 13;
    reg |= 4 << 14;
    reg |= 1 << 17;
    msi001_wreg(reg);

    ESP_LOGI(MSI001_TAG, "reg1: %06X", reg);

    return 0;
};

static void msi001_set_am_filter(uint32_t freq)
{
    static const struct {
        uint32_t rf;
        rf_switch_band_t filter;
    } am_filter_lut[]={
            {12000000, RF_BAND_0_12_MHz},
            {30000000, RF_BAND_12_30MHz},
            {60000000, RF_BAND_30_60_MHz}
    };

    rf_switch_band_t am_filter = RF_BAND_0_12_MHz;

    for(int i = 0 ; i < ARRAY_SIZE(am_filter_lut); i++) {
        if(freq <= am_filter_lut[i].rf) {
            am_filter = am_filter_lut[i].filter;
            break;
        }
    }

    rf_switch_set_band(am_filter);
}

int msi001_set_tuner_frequency(uint32_t freq)
{
    uint8_t  mode = 0x42;
    uint8_t  lo_div = 32;
    rf_switch_mode_t rf_band = RF_SWITCH_VHF1_MODE;

    /* find band index */
    static const struct {
        uint32_t rf;
        uint8_t mode;
        uint8_t div_lo;
        rf_switch_mode_t rf_mode;
    } band_lut[] = {
            {50000000,  0xe1, 16, RF_SWITCH_AM_MODE},   /* AM_MODE2, antenna 2 */
            {108000000, 0x42, 32, RF_SWITCH_VHF1_MODE}, /* VHF_MODE */
            {330000000, 0x44, 16, RF_SWITCH_VHF2_MODE}, /* B3_MODE */
            {960000000, 0x48, 4, RF_SWITCH_B45_MODE},   /* B45_MODE */
            {~0U,       0x50, 2, RF_SWITCH_LBAND_MODE}  /* BL_MODE */
    };

    for (int i = 0; i < ARRAY_SIZE(band_lut); i++) {
        if (freq <= band_lut[i].rf) {
            mode    = band_lut[i].mode;
            lo_div  = band_lut[i].div_lo;
            rf_band = band_lut[i].rf_mode;
            break;
        }
    }
    /* set rf band switch */
    rf_switch_set_mode(rf_band);
    ESP_LOGI(MSI001_TAG, "Set rf band to: %d", rf_band);

    /* set am filter switch */
    if(rf_band == RF_SWITCH_AM_MODE) {
        msi001_set_am_filter(freq);
    }

    /* reg0 value */
    uint32_t reg0 = 0;
    reg0 |= mode << 4;
    reg0 |= 0x03 << 12; /* Zero IF */
    reg0 |= 0x00 << 14; /* IF bandwidth: 200K */
    reg0 |= 0x02 << 17; /* fref: 24.576MHz */
    reg0 |= 0x00 << 20;

    uint64_t f_synth = 0;
    uint32_t f_if    = 0;
    uint32_t f_if1   = 122880000;
    uint32_t fref    = 24576000;

    if(rf_band == RF_SWITCH_AM_MODE) {
        //AM
        f_synth = (freq + f_if + f_if1)*lo_div;
    } else {
        f_synth = (freq + f_if)*lo_div;
    }

    uint32_t f_step = 1;

    uint32_t k_thresh = (fref * 4)/(lo_div * f_step);
    uint64_t k_int = f_synth/(fref*4);
    uint64_t k_frac = ((f_synth*k_thresh)/(fref * 4))-k_int*k_thresh;

    uint64_t k = gcd(k_frac, k_thresh);

    k_frac   /= k;
    k_thresh /= k;

    uint32_t reg5 = 5 << 0;
    reg5 |= k_thresh << 4;
    reg5 |= 1 << 19;
    reg5 |= 1 << 21;

    uint32_t reg2 = 2 << 0;
    reg2 |= k_frac<< 4;
    reg2 |= k_int << 16;

    msi001_wreg(0x0e);
    msi001_wreg(reg0);
    msi001_wreg(reg5);
    msi001_wreg(reg2);
    msi001_wreg(0x140a1);
    msi001_wreg(0x3FFFF6);
    msi001_wreg(0x03);

    ESP_LOGI(MSI001_TAG, "reg0: %06X", reg0);
    ESP_LOGI(MSI001_TAG, "reg5: %06X", reg5);
    ESP_LOGI(MSI001_TAG, "reg2: %06X", reg2);

    return 0;
}

void spi_pre_transfer_callback(spi_transaction_t *t)
{

}

static int msi001_spi_init() {
    esp_err_t ret;
    spi_bus_config_t buscfg={
            .miso_io_num=PIN_NUM_MISO,
            .mosi_io_num=PIN_NUM_MOSI,
            .sclk_io_num=PIN_NUM_CLK,
            .quadwp_io_num=-1,
            .quadhd_io_num=-1,
            .max_transfer_sz=8
    };

    spi_device_interface_config_t devcfg={
            .clock_speed_hz=1*1000*100,   //Clock out at 0.1 MHz
            .mode=0,                            //SPI mode 0
            .spics_io_num=PIN_NUM_CS,           //CS pin
            .queue_size=7,                      //We want to be able to queue 7 transactions at a time
            .pre_cb=spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(SPI_HOST, &buscfg, 0);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(SPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    return ret;
}

int msi001_standby() {
    //msi001_spi_init();
    soft_spi_init();
    msi001_wreg(0x0e);
    msi001_wreg(0x43420); /* low power IF mode */
    msi001_wreg(0x2800f5);
    msi001_wreg(0x200012);
    msi001_wreg(0x140a1);
    msi001_wreg(0x3FFFF6);
    msi001_wreg(0x7c03);

    ESP_LOGI(MSI001_TAG, "msi001 init ok");
    return 0;
}
