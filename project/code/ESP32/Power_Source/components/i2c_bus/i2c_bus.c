#include <stdio.h>
#include "esp_err.h"
#include "driver/i2c.h"
#include "i2c_bus.h"
#include "freertos/semphr.h"

SemaphoreHandle_t i2c_bus_mux = NULL;

esp_err_t i2c_bus_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data, uint32_t timeout_ticks)
{
    int res = 0;
    xSemaphoreTake(i2c_bus_mux, portMAX_DELAY);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, slave_addr<<1, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_write_byte(cmd, data, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, timeout_ticks);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(i2c_bus_mux);
    return res ? ESP_FAIL: ESP_OK;
}

esp_err_t i2c_bus_read_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, uint32_t timeout_ticks)
{
    int res = 0;
    xSemaphoreTake(i2c_bus_mux, portMAX_DELAY);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, slave_addr<<1, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, (slave_addr<<1) | 0x01, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_read_byte(cmd, pdata, 0x01/*NACK_VAL*/);
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, timeout_ticks - 100);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(i2c_bus_mux);
    return res ? ESP_FAIL: ESP_OK;
}

esp_err_t i2c_bus_write_data(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, size_t len, uint32_t timeout_ticks)
{
    int x;
    int res = 0;
    xSemaphoreTake(i2c_bus_mux, portMAX_DELAY);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, (slave_addr<<1), 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    for (x = 0; x < len; x++) {
        res |= i2c_master_write_byte(cmd, &pdata[x], 1 /*ACK_CHECK_EN*/);
    }
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, timeout_ticks);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(i2c_bus_mux);
    return res ? ESP_FAIL: ESP_OK;
}

esp_err_t i2c_bus_read_data(uint8_t slave_addr, uint8_t reg_addr, uint8_t *pdata, size_t len, uint32_t timeout_ticks)
{
    int x;
    int res = 0;
    xSemaphoreTake(i2c_bus_mux, portMAX_DELAY);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, (slave_addr<<1) | 0x01, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_write_byte(cmd, reg_addr, 1 /*ACK_CHECK_EN*/);
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, 100 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    res |= i2c_master_start(cmd);
    res |= i2c_master_write_byte(cmd, (slave_addr<<1) | 0x01, 1 /*ACK_CHECK_EN*/);
    for (x = 0; x < len - 1; x++) {
        res |= i2c_master_read_byte(cmd, &pdata[x], 0x00/*ACK_VAL*/);
    }
    res |= i2c_master_read_byte(cmd, &pdata[x], 0x01/*NACK_VAL*/);
    res |= i2c_master_stop(cmd);
    res |= i2c_master_cmd_begin(0, cmd, timeout_ticks);
    i2c_cmd_link_delete(cmd);
    xSemaphoreGive(i2c_bus_mux);
    return res ? ESP_FAIL: ESP_OK;
}

esp_err_t i2c_bus_init()
{
    int res;
    int i2c_master_port = I2C_NUM_0;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_BUS_SCL_PIN;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = (gpio_num_t)I2C_BUS_SDA_PIN;
    conf.scl_pullup_en = 1;
    conf.master.clk_speed = 100000;
    res = i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
    res |= i2c_param_config(i2c_master_port, &conf);
    

    i2c_bus_mux = xSemaphoreCreateMutex();
    return res ? ESP_FAIL: ESP_OK;
}
