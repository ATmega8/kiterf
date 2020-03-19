#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tca9555.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lcd.h"
#include "esp_lua.h"

#include "waterfall.h"
#include "msi001.h"
#include "rfswitch.h"
#include "cs5361.h"
#include "lsdr.h"

extern lv_img_dsc_t waterfall;

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

    uint32_t waterfallTimer = xTaskGetTickCount();

    while (1) {
        if((xTaskGetTickCount() - waterfallTimer) > ( 100 / portTICK_PERIOD_MS)) {
            waterfallTimer = xTaskGetTickCount();
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

    xTaskCreate(lvgl_task, "lvgl_task", 1024 * 4, NULL, 5, NULL);
    xTaskCreate(lua_task, "lua_task", 1024 * 10, NULL, 6, NULL);

    vTaskSuspend(NULL);
}
