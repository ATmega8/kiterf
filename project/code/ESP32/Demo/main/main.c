#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tca9555.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lcd.h"
#include "esp_lua.h"

#include "waterfall.h"
#include "msi001.h"
#include "rfswitch.h"
#include "cs5361.h"
#include "lsdr.h"
#include "iq_receive.h"

extern lv_img_dsc_t waterfall;
uint8_t iq_receive_buff[1024*4];

xSemaphoreHandle iq_data_ready_sem = NULL;

#define IQ_TIMING_IO  11
#define FFT_TIMING_IO 12

void IRAM_ATTR lvgl_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t* color_p)
{
    int32_t x;
    uint32_t len = (y2 - y1 + 1)*(x2 - x1 + 1);

    lcd_set_index(x1, y1, x2, y2);
    for (x = 0; x < len / LCD_BURST_MAX_LEN; x++) {
        lcd_write_data((uint16_t *)color_p, LCD_BURST_MAX_LEN);
        color_p +=  LCD_BURST_MAX_LEN;
    }
    lcd_write_data((uint16_t *)color_p, len % LCD_BURST_MAX_LEN);

    lv_flush_ready();
}

void lvgl_task(void *arg)
{
    lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
    disp_drv.disp_flush = lvgl_disp_flush;     /*Set your driver function*/
    lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/

    lv_obj_t* waterfall_image = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(waterfall_image, &waterfall);
    lv_obj_align(waterfall_image, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

    while (1) {

        if(xSemaphoreTake(iq_data_ready_sem, portMAX_DELAY) == pdTRUE) {
            //gpio_set_level(FFT_TIMING_IO, 1);
            waterfall_do_fft(iq_receive_buff);
            //gpio_set_level(FFT_TIMING_IO, 0);
            lvgl_update_waterfall(waterfall_image);
        }

        lv_task_handler();
        vTaskDelay(10/portTICK_RATE_MS);
    }
}

void lua_task(void* parameters)
{
    static const luaL_Reg local_libs[] = {
            {"sdr", luaopen_lsdr},
            {NULL, NULL}
    };

    const char *ESP_LUA_ARGV[2] = {"./lua", NULL};
    esp_lua_init(NULL, NULL, local_libs);

    while (1) {
        esp_lua_main(1, ESP_LUA_ARGV);
    }
}

void iq_receive_task(void* parameters)
{
    iq_receive_spi_init();

    while(1) {
        //gpio_set_level(IQ_TIMING_IO, 1);
        iq_receive_run(iq_receive_buff, 1024);
        xSemaphoreGive(iq_data_ready_sem);
        //gpio_set_level(IQ_TIMING_IO, 0);
    }
}

void app_main()
{
    lv_init();
    lcd_init();
    tac9555_i2c_init();
    vTaskDelay(100);
    tca9555_init();
    msi001_standby();
    rf_switch_set_mode(RF_SWITCH_VHF1_MODE);
    cs5361_init();

    /*gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL<<IQ_TIMING_IO) | (1ULL<<FFT_TIMING_IO));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);*/

    iq_data_ready_sem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(lvgl_task, "lvgl_task", 1024 * 4, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(iq_receive_task, "iq_task", 1024 * 4, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(lua_task, "lua_task", 1024 * 10, NULL, 4, NULL, 0);

    vTaskSuspend(NULL);
}
