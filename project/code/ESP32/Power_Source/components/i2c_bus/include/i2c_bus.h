#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_BUS_SCL_PIN 16
#define I2C_BUS_SDA_PIN 17

esp_err_t i2c_bus_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data, uint32_t timeout_ticks);

esp_err_t i2c_bus_read_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, uint32_t timeout_ticks);

esp_err_t i2c_bus_write_data(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, size_t len, uint32_t timeout_ticks);

esp_err_t i2c_bus_read_data(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, size_t len, uint32_t timeout_ticks);

esp_err_t i2c_bus_init();

#ifdef __cplusplus
}
#endif
