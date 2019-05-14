
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "rf_spi_master.h"

typedef struct {
    uint8_t *data;
    int len;
} rf_spi_master_event_t;

typedef struct {
    int trans_max_len;
    rf_spi_master_callback_t trans_start_cb;
    rf_spi_master_callback_t trans_done_cb;
    QueueHandle_t send_queue;
    QueueHandle_t recv_queue;
    uint8_t *send_dma_buf;
    uint8_t *recv_dma_buf;
} rf_spi_master_obj_t;

rf_spi_master_obj_t *rf_spi_master_obj = NULL;

static int inline rf_spi_master_trans(uint8_t *data, int len, uint32_t *timeout_ticks)
{
    int x;
    int ret = 0;
    rf_spi_master_event_t event;
    uint32_t ticks_escape, ticks_last;
    if (len == 0) {
        return 0;
    }
    event.data = data;
    event.len = len;
    ticks_last = clock() * portTICK_RATE_MS;
    ret = xQueueSend(rf_spi_master_obj->send_queue, (void *)&event, *timeout_ticks);
    if (*timeout_ticks != portMAX_DELAY) {
        ticks_escape = clock() * portTICK_RATE_MS - ticks_last;
        if (*timeout_ticks <= ticks_escape) {
            return -1;
        } else {
            *timeout_ticks -= ticks_escape;
        }
    }
    if (ret == pdFALSE) {
        return -1;
    }
    ret = xQueueReceive(rf_spi_master_obj->recv_queue, &event, *timeout_ticks);
    if (*timeout_ticks != portMAX_DELAY) {
        ticks_escape = clock() * portTICK_RATE_MS - ticks_last;
        if (*timeout_ticks <= ticks_escape) {
            return -1;
        } else {
            *timeout_ticks -= ticks_escape;
        }
    }
    if (ret == pdFALSE || event.data == NULL) {
        return -1;
    }
    for (x = 0; x < event.len; x++) {
        data[x] = event.data[x];
    } 

    return event.len;
}

int rf_spi_master_send_recv(uint8_t *data, int len, uint32_t timeout_ticks)
{
    int ret = 0;
    int ret_len = 0;
    int x;
    if (rf_spi_master_obj == NULL || data == NULL || len < 0) {
        printf("argc error: %s, %d\n", __func__, __LINE__);
        return -1;
    }

    for (x = 0; x < len / rf_spi_master_obj->trans_max_len; x++) {
        ret = rf_spi_master_trans(data, rf_spi_master_obj->trans_max_len, &timeout_ticks);
        if (ret < 0) {
            printf("trans error: %s, %d\n", __func__, __LINE__);
            return -1;
        } else {
            ret_len += ret;
        }
        data += rf_spi_master_obj->trans_max_len;
    }
    ret = rf_spi_master_trans(data, len % rf_spi_master_obj->trans_max_len, &timeout_ticks);
    if (ret < 0) {
        printf("trans error: %s, %d\n", __func__, __LINE__);
        return -1;
    } else {
        ret_len += ret;
    }

    return ret_len;
}

void rf_spi_master_task(void *arg)
{
    int x;
    esp_err_t ret;
    spi_device_handle_t handle;
    
    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = RF_SPI_MASTER_PIN_MOSI,
        .miso_io_num = RF_SPI_MASTER_PIN_MISO,
        .sclk_io_num = RF_SPI_MASTER_PIN_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = rf_spi_master_obj->trans_max_len
    };

    spi_device_interface_config_t devcfg={
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 5000000,
        .duty_cycle_pos = 128,        //50% duty cycle
        .mode = 0,
        .spics_io_num = RF_SPI_MASTER_PIN_CS,
        .cs_ena_posttrans = 3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
        .queue_size = 3
    };

    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);

    spi_transaction_t t;
    rf_spi_master_event_t event;
    memset(&t, 0, sizeof(t));
    t.tx_buffer = rf_spi_master_obj->send_dma_buf;
    t.rx_buffer = rf_spi_master_obj->recv_dma_buf;
    while (1) {
        xQueueReceive(rf_spi_master_obj->send_queue, &event, portMAX_DELAY);
        if (event.data == NULL || event.len <= 0) {
            continue;
        }
        for (x = 0; x < event.len; x++) {
            rf_spi_master_obj->send_dma_buf[x] = event.data[x];
        }
        memset(rf_spi_master_obj->recv_dma_buf, 0, rf_spi_master_obj->trans_max_len);
        t.length = event.len * 8;
        if (rf_spi_master_obj->trans_start_cb) {
            rf_spi_master_obj->trans_start_cb(event.len);
        }
        ret = spi_device_transmit(handle, &t);
        if (rf_spi_master_obj->trans_done_cb) {
            rf_spi_master_obj->trans_done_cb(event.len);
        }
        if (ret != ESP_OK) {
            event.data = NULL;
        } else {
            event.data = rf_spi_master_obj->recv_dma_buf;
        }
        xQueueSend(rf_spi_master_obj->recv_queue, (void *)&event, portMAX_DELAY);
    }
}

int rf_spi_master_deinit()
{
    // TODO

    return 0;
}

int rf_spi_master_init(rf_spi_master_config_t *config)
{
    if (config->trans_max_len <= 0) {
        printf("argc error: %s, %d\n", __func__, __LINE__);
        return -1;
    }
    rf_spi_master_obj = (rf_spi_master_obj_t *)heap_caps_calloc(1, sizeof(rf_spi_master_obj_t), MALLOC_CAP_8BIT);
    rf_spi_master_obj->trans_max_len = config->trans_max_len;
    rf_spi_master_obj->trans_start_cb = config->trans_start_cb;
    rf_spi_master_obj->trans_done_cb = config->trans_done_cb;
    rf_spi_master_obj->send_queue = xQueueCreate(1, sizeof(rf_spi_master_event_t));
    rf_spi_master_obj->recv_queue = xQueueCreate(1, sizeof(rf_spi_master_event_t));
    rf_spi_master_obj->send_dma_buf = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*rf_spi_master_obj->trans_max_len, MALLOC_CAP_DMA);
    rf_spi_master_obj->recv_dma_buf = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*rf_spi_master_obj->trans_max_len, MALLOC_CAP_DMA);

    xTaskCreate(rf_spi_master_task, "rf_spi_master_task", 1024 * 4, NULL, 5, NULL);
    return 0;
}