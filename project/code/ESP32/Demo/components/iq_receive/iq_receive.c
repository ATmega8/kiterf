//
// Created by life on 20-3-22.
//
#include <string.h>

#include "iq_receive.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"

static const char* IQ_RECEIVER_TAG="IQ_Receiver";

#define PIN_NUM_MISO -1
#define PIN_NUM_MOSI 12
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   27

//Called after a transaction is queued and ready for pickup by master. We use this to set the handshake line high.
void my_post_setup_cb(spi_slave_transaction_t *trans) {
}

//Called after transaction is sent/received. We use this to set the handshake line low.
void my_post_trans_cb(spi_slave_transaction_t *trans) {
}

void iq_receive_spi_init(void)
{
    esp_err_t ret;

    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
            .mosi_io_num=PIN_NUM_MOSI,
            .miso_io_num=PIN_NUM_MISO,
            .sclk_io_num=PIN_NUM_CLK
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
            .mode=0,
            .spics_io_num=PIN_NUM_CS,
            .queue_size=3,
            .flags=0,
            .post_setup_cb=my_post_setup_cb,
            .post_trans_cb=my_post_trans_cb
    };

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret=spi_slave_initialize(VSPI_HOST, &buscfg, &slvcfg, 2);
    assert(ret==ESP_OK);
}

void iq_receive_run(uint8_t* pdata, size_t len)
{
    esp_err_t ret;
    spi_slave_transaction_t t;

    for (int i = 0; i < 4; ++i) {
        memset(&t, 0, sizeof(t));
        t.length = 8*len;
        t.rx_buffer = pdata+len*i;
        t.tx_buffer = pdata+len*i;

        ret=spi_slave_transmit(VSPI_HOST, &t, portMAX_DELAY);
        ESP_ERROR_CHECK(ret);
    }
}
