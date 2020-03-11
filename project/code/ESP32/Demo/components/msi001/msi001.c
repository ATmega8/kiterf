//
// Created by life on 19-11-20.
//

#include "msi001.h"
#include <string.h>

#include "driver/spi_master.h"

// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Mirics MSi001 silicon tuner driver
 *
 * Copyright (C) 2013 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2014 Antti Palosaari <crope@iki.fi>
 */

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

spi_device_handle_t  spi;

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

static void delayUS(uint32_t us) {
    while (us--) {

    }
}

static int spi_write(uint8_t *pdata, uint8_t len) {
    esp_err_t ret;
    spi_transaction_t t;
    if (len==0) return 0;             //no need to send anything

    memset(&t, 0, sizeof(t));       //Zero out the transaction

    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=pdata;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_polling_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.
    return 0;
}

static int msi001_wreg(uint32_t data) {
    uint8_t buf[3];
    buf[2] = (uint8_t) (data & 0x000000FF);
    buf[1] = (uint8_t) ((data & 0x0000FF00) >> 8);
    buf[0] = (uint8_t) ((data & 0x00FF0000) >> 16);
    /* Register format: 4 bits addr + 20 bits value */
    return spi_write(buf, 3);
};

static int msi001_set_gain(msi001_t *dev) {
    int ret;
    uint32_t reg;

    reg = 1 << 0;
    reg |= (59 - dev->if_gain) << 4;
    reg |= 0 << 10;
    reg |= (1 - dev->mixer_gain) << 12;
    reg |= (1 - dev->lna_gain) << 13;
    reg |= 4 << 14;
    reg |= 0 << 17;
    ret = msi001_wreg(reg);

    if (ret) {
        goto err;
    }

    return 0;
    err:
    return ret;
};

int msi001_set_tuner(msi001_t *dev) {
    int ret, i;
    uint32_t uitmp, div_n, k, k_thresh, k_frac, div_lo = 1, f_if1;
    uint32_t reg;
    uint64_t f_vco;
    uint8_t mode = 0, filter_mode;

    static const struct {
        uint32_t rf;
        uint8_t mode;
        uint8_t div_lo;
    } band_lut[] = {
            {50000000,  0xe1, 16}, /* AM_MODE2, antenna 2 */
            {108000000, 0x42, 32}, /* VHF_MODE */
            {330000000, 0x44, 16}, /* B3_MODE */
            {960000000, 0x48, 4}, /* B45_MODE */
            {~0U,       0x50, 2}, /* BL_MODE */
    };
    static const struct {
        uint32_t freq;
        uint8_t filter_mode;
    } if_freq_lut[] = {
            {0,       0x03}, /* Zero IF */
            {450000,  0x02}, /* 450 kHz IF */
            {1620000, 0x01}, /* 1.62 MHz IF */
            {2048000, 0x00}, /* 2.048 MHz IF */
    };
    static const struct {
        uint32_t freq;
        uint8_t val;
    } bandwidth_lut[] = {
            {200000,  0x00}, /* 200 kHz */
            {300000,  0x01}, /* 300 kHz */
            {600000,  0x02}, /* 600 kHz */
            {1536000, 0x03}, /* 1.536 MHz */
            {5000000, 0x04}, /* 5 MHz */
            {6000000, 0x05}, /* 6 MHz */
            {7000000, 0x06}, /* 7 MHz */
            {8000000, 0x07}, /* 8 MHz */
    };

    uint32_t f_rf = dev->f_rf;

    /*
     * bandwidth (Hz)
     * 200000, 300000, 600000, 1536000, 5000000, 6000000, 7000000, 8000000
     */
    unsigned int bandwidth;

    /*
     * intermediate frequency (Hz)
     * 0, 450000, 1620000, 2048000
     */
    unsigned int f_if = 0;
#define F_REF 24576000
#define DIV_PRE_N 4
#define    F_VCO_STEP 100000 /*1KHz*/

    for (i = 0; i < ARRAY_SIZE(band_lut); i++) {
        if (f_rf <= band_lut[i].rf) {
            mode = band_lut[i].mode;
            div_lo = band_lut[i].div_lo;
            break;
        }
    }

    if (i == ARRAY_SIZE(band_lut)) {
        ret = -1;
        goto err;
    }

    /* AM_MODE is upconverted */
    if ((mode >> 0) & 0x1) {
        f_if1 = 5 * F_REF;
    } else {
        f_if1 = 0;
    }

    i = dev->f_if;

    if (i >= ARRAY_SIZE(if_freq_lut)) {
        ret = -1;
        goto err;
    }

    filter_mode = if_freq_lut[i].filter_mode;
    f_if = if_freq_lut[i].freq;


    /* filters */
    i = dev->filter;

    if (i >= ARRAY_SIZE(bandwidth_lut)) {
        ret = -1;
        goto err;
    }

    bandwidth = bandwidth_lut[i].val;

    /*
     * Fractional-N synthesizer
     *
     *           +---------------------------------------+
     *           v                                       |
     *  Fref   +----+     +-------+         +----+     +------+     +---+
     * ------> | PD | --> |  VCO  | ------> | /4 | --> | /N.F | <-- | K |
     *         +----+     +-------+         +----+     +------+     +---+
     *                      |
     *                      |
     *                      v
     *                    +-------+  Fout
     *                    | /Rout | ------>
     *                    +-------+
     */

    /* Calculate PLL integer and fractional control word. */
    f_vco = (uint64_t) (f_rf + f_if + f_if1) * div_lo;
    div_n = div_u64_rem(f_vco, DIV_PRE_N * F_REF, &k);
    k_thresh = (DIV_PRE_N * F_REF) / (F_VCO_STEP * div_lo);
    k_frac = div_u64((uint64_t) k * k_thresh, (DIV_PRE_N * F_REF));

    /* Find out greatest common divisor and divide to smaller. */
    uitmp = gcd(k_thresh, k_frac);
    k_thresh /= uitmp;
    k_frac /= uitmp;

    /* Force divide to reg max. Resolution will be reduced. */
    uitmp = DIV_ROUND_UP(k_thresh, 4095);
    k_thresh = DIV_ROUND_CLOSEST(k_thresh, uitmp);
    k_frac = DIV_ROUND_CLOSEST(k_frac, uitmp);

    /* Calculate real RF set. */
    uitmp = (unsigned int) F_REF * DIV_PRE_N * div_n / div_lo;
    uitmp += (unsigned int) F_REF * DIV_PRE_N * k_frac / (k_thresh * div_lo);
    uitmp /= div_lo;
    dev->f_real_rf = uitmp;

    reg = 0 << 0;
    reg |= mode << 4;
    reg |= filter_mode << 12;
    reg |= bandwidth << 14;
    reg |= 0x02 << 17;
    reg |= 0x00 << 20;
    ret = msi001_wreg(reg);

    if (ret) {
        goto err;
    }

    reg = 5 << 0;
    reg |= k_thresh << 4;
    reg |= 1 << 19;
    reg |= 1 << 21;
    ret = msi001_wreg(reg);

    if (ret) {
        goto err;
    }

    reg = 2 << 0;
    reg |= k_frac << 4;
    reg |= div_n << 16;
    ret = msi001_wreg(reg);

    if (ret) {
        goto err;
    }

    /*ret = msi001_set_gain(dev);

    if (ret) {
        goto err;
    }

    reg = 6 << 0;
    reg |= 63 << 4;
    reg |= 4095 << 10;
    ret = msi001_wreg(reg);

    if (ret) {
        goto err;
    }

    ret = msi001_wreg(0x000004);

    if (ret) {
        goto err;
    }

    ret = msi001_wreg(0x000003);

    if (ret) {
        goto err;
    }*/

    return 0;
    err:
    return ret;
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
            .clock_speed_hz=1*1000*1000,  //Clock out at 1 MHz
            .mode=0,                            //SPI mode 0
            .spics_io_num=PIN_NUM_CS,           //CS pin
            .queue_size=7,                      //We want to be able to queue 7 transactions at a time
            .pre_cb=spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };

    //Initialize the SPI bus
    ret=spi_bus_initialize(VSPI_HOST, &buscfg, 2);
    ESP_ERROR_CHECK(ret);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(VSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);
    return ret;
}

int msi001_standby() {
    msi001_spi_init();

    msi001_wreg(0x43420); /* low power IF mode */
    msi001_wreg(0x2800f5);
    msi001_wreg(0x200012);
    msi001_wreg(0x140a1);
    msi001_wreg(0x3FFFF6);
    msi001_wreg(0x7c03);
    return 0;
}
