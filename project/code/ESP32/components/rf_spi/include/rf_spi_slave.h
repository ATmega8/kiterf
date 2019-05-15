#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RF_SPI_SLAVE_PIN_SCLK 33
#define RF_SPI_SLAVE_PIN_MISO 25
#define RF_SPI_SLAVE_PIN_MOSI 26
#define RF_SPI_SLAVE_PIN_CS 19

typedef void (*rf_spi_slave_callback_t)(size_t len);

typedef struct {
    size_t trans_max_len;
    rf_spi_slave_callback_t trans_start_cb;
    rf_spi_slave_callback_t trans_done_cb;
} rf_spi_slave_config_t;

int rf_spi_slave_send_recv(void *data, size_t len, uint32_t timeout_ticks);

int rf_spi_slave_deinit();

int rf_spi_slave_init(rf_spi_slave_config_t *config);

#ifdef __cplusplus
}
#endif