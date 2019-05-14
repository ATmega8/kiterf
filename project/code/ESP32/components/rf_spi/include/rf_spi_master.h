#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RF_SPI_MASTER_PIN_SCLK 33
#define RF_SPI_MASTER_PIN_MISO 25
#define RF_SPI_MASTER_PIN_MOSI 26
#define RF_SPI_MASTER_PIN_CS 19


typedef void (*rf_spi_master_callback_t)(int len);

typedef struct {
    int trans_max_len;
    rf_spi_master_callback_t trans_start_cb;
    rf_spi_master_callback_t trans_done_cb;
} rf_spi_master_config_t;

int rf_spi_master_send_recv(uint8_t *data, int len, uint32_t timeout_ticks);

int rf_spi_master_deinit();

int rf_spi_master_init(rf_spi_master_config_t *config);

#ifdef __cplusplus
}
#endif