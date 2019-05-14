
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "rf_spi_slave.h"

typedef struct {
    uint8_t *data;
    int len;
} rf_spi_slave_event_t;

typedef struct {
    int trans_max_len;
    rf_spi_slave_callback_t trans_start_cb;
    rf_spi_slave_callback_t trans_done_cb;
    QueueHandle_t send_queue;
    QueueHandle_t recv_queue;
    uint8_t *send_dma_buf;
    uint8_t *recv_dma_buf;
} rf_spi_slave_obj_t;

rf_spi_slave_obj_t *rf_spi_slave_obj = NULL;

static void IRAM_ATTR rf_spi_slave_trans_setup_cb(spi_slave_transaction_t *trans) 
{
    if (rf_spi_slave_obj->trans_start_cb) {
        rf_spi_slave_obj->trans_start_cb(trans->length / 8);
    }
}


static void IRAM_ATTR rf_spi_slave_trans_post_cb(spi_slave_transaction_t *trans) 
{
    if (rf_spi_slave_obj->trans_done_cb) {
        rf_spi_slave_obj->trans_done_cb(trans->length / 8);
    }
}

static int inline rf_spi_slave_trans(uint8_t *data, int len, uint32_t *timeout_ticks)
{
    int x;
    int ret = 0;
    rf_spi_slave_event_t event;
    uint32_t ticks_escape, ticks_last;
    if (len == 0) {
        return 0;
    }
    event.data = data;
    event.len = len;
    ticks_last = clock() * portTICK_RATE_MS;
    ret = xQueueSend(rf_spi_slave_obj->send_queue, (void *)&event, *timeout_ticks);
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
    ret = xQueueReceive(rf_spi_slave_obj->recv_queue, &event, *timeout_ticks);
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

int rf_spi_slave_send_recv(uint8_t *data, int len, uint32_t timeout_ticks)
{
    int ret = 0;
    int ret_len = 0;
    int x;
    if (rf_spi_slave_obj == NULL || data == NULL || len < 0) {
        printf("argc error: %s, %d\n", __func__, __LINE__);
        return -1;
    }

    for (x = 0; x < len / rf_spi_slave_obj->trans_max_len; x++) {
        ret = rf_spi_slave_trans(data, rf_spi_slave_obj->trans_max_len, &timeout_ticks);
        if (ret < 0) {
            printf("trans error: %s, %d\n", __func__, __LINE__);
            return -1;
        } else {
            ret_len += ret;
        }
        data += rf_spi_slave_obj->trans_max_len;
    }
    ret = rf_spi_slave_trans(data, len % rf_spi_slave_obj->trans_max_len, &timeout_ticks);
    if (ret < 0) {
        printf("trans error: %s, %d\n", __func__, __LINE__);
        return -1;
    } else {
        ret_len += ret;
    }

    return ret_len;
}

void rf_spi_slave_task(void *arg)
{
    int x;
    esp_err_t ret;
    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = RF_SPI_SLAVE_PIN_MOSI,
        .miso_io_num = RF_SPI_SLAVE_PIN_MISO,
        .sclk_io_num = RF_SPI_SLAVE_PIN_SCLK,
        .max_transfer_sz = rf_spi_slave_obj->trans_max_len
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = RF_SPI_SLAVE_PIN_CS,
        .queue_size = 3,
        .flags =0
    };

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(RF_SPI_SLAVE_PIN_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(RF_SPI_SLAVE_PIN_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(RF_SPI_SLAVE_PIN_CS, GPIO_PULLUP_ONLY);

    //Initialize SPI slave interface
    ret=spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
    assert(ret == ESP_OK);
    spi_slave_transaction_t t;
    rf_spi_slave_event_t event;
    memset(&t, 0, sizeof(t));
    t.tx_buffer = rf_spi_slave_obj->send_dma_buf;
    t.rx_buffer = rf_spi_slave_obj->recv_dma_buf;
    while (1) {
        xQueueReceive(rf_spi_slave_obj->send_queue, &event, portMAX_DELAY);
        if (event.data == NULL || event.len <= 0) {
            continue;
        }
        for (x = 0; x < event.len; x++) {
            rf_spi_slave_obj->send_dma_buf[x] = event.data[x];
        }
        memset(rf_spi_slave_obj->recv_dma_buf, 0, rf_spi_slave_obj->trans_max_len);
        t.length = event.len * 8;
        if (rf_spi_slave_obj->trans_start_cb) {
            rf_spi_slave_obj->trans_start_cb(event.len);
        }
        ret = spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
        if (rf_spi_slave_obj->trans_done_cb) {
            rf_spi_slave_obj->trans_done_cb(event.len);
        }
        if (ret != ESP_OK) {
            event.data = NULL;
        } else {
            event.data = rf_spi_slave_obj->recv_dma_buf;
        }
        xQueueSend(rf_spi_slave_obj->recv_queue, (void *)&event, portMAX_DELAY);
    }
}

int rf_spi_slave_deinit()
{
    // TODO

    return 0;
}

int rf_spi_slave_init(rf_spi_slave_config_t *config)
{
    if (config->trans_max_len <= 0) {
        printf("argc error: %s, %d\n", __func__, __LINE__);
        return -1;
    }
    rf_spi_slave_obj = (rf_spi_slave_obj_t *)heap_caps_calloc(1, sizeof(rf_spi_slave_obj_t), MALLOC_CAP_8BIT);
    rf_spi_slave_obj->trans_max_len = config->trans_max_len;
    rf_spi_slave_obj->trans_start_cb = config->trans_start_cb;
    rf_spi_slave_obj->trans_done_cb = config->trans_done_cb;
    rf_spi_slave_obj->send_queue = xQueueCreate(1, sizeof(rf_spi_slave_event_t));
    rf_spi_slave_obj->recv_queue = xQueueCreate(1, sizeof(rf_spi_slave_event_t));
    rf_spi_slave_obj->send_dma_buf = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*rf_spi_slave_obj->trans_max_len, MALLOC_CAP_DMA);
    rf_spi_slave_obj->recv_dma_buf = (uint8_t *)heap_caps_malloc(sizeof(uint8_t)*rf_spi_slave_obj->trans_max_len, MALLOC_CAP_DMA);

    xTaskCreate(rf_spi_slave_task, "rf_spi_slave_task", 1024 * 4, NULL, 5, NULL);
    return 0;
}