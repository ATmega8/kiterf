#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "dsp_math.h"
#include "lvgl.h"
#include "lcd.h"
#include "gui.h"

lv_disp_t *disp[2];

void IRAM_ATTR disp_flush1(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint32_t len = (sizeof(uint16_t) * ((area->y2 - area->y1 + 1)*(area->x2 - area->x1 + 1)));

    lcd_select(0);
    lcd_set_index(area->x1, area->y1, area->x2, area->y2);
    lcd_write_data((uint16_t *)color_p, len);

    lv_disp_flush_ready(disp_drv);
}

void IRAM_ATTR disp_flush2(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint32_t len = (sizeof(uint16_t) * ((area->y2 - area->y1 + 1)*(area->x2 - area->x1 + 1)));

    if (area->x1 < 160 && area->x2 < 160 && area->y1 < 80 && area->y2 < 80) {
        lcd_select(1);
        lcd_set_index(area->x1, area->y1, area->x2, area->y2);
        lcd_write_data((uint16_t *)color_p, len);
    }

    lv_disp_flush_ready(disp_drv);
}

void memory_monitor(lv_task_t * param)
{
    (void) param; /*Unused*/

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d, system free: %d\n", (int)mon.total_size - mon.free_size,
           mon.used_pct,
           mon.frag_pct,
           (int)mon.free_biggest_size,
           esp_get_free_heap_size());
}

static void gui_tick_task(void * arg)
{
    while(1) {
        lv_tick_inc(10);
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void gui_task(void *arg)
{
    lcd_init();

    xTaskCreate(gui_tick_task, "gui_tick_task", 512, NULL, 5, NULL);

    lv_init();

    /*Create a display buffer*/
    static lv_disp_buf_t disp_buf1;
    static lv_color_t *buf1_1 = NULL;
    buf1_1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * (240 * 240), MALLOC_CAP_DMA);
    lv_disp_buf_init(&disp_buf1, buf1_1, NULL, 240 * 240);

    /*Create a display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = disp_flush1;    /*Used when `LV_VDB_SIZE != 0` in lv_conf.h (buffered drawing)*/
    disp[0] = lv_disp_drv_register(&disp_drv);

    /*Create an other buffer for double buffering*/
    static lv_disp_buf_t disp_buf2;
    static lv_color_t *buf2_1 = NULL;
    buf2_1 = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * (160 * 80), MALLOC_CAP_DMA);
    lv_disp_buf_init(&disp_buf2, buf2_1, NULL, 160 * 80);

    /*Create an other display*/
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.buffer = &disp_buf2;
    disp_drv.flush_cb = disp_flush2;    /*Used when `LV_VDB_SIZE != 0` in lv_conf.h (buffered drawing)*/
    disp_drv.hor_res = 160;
    disp_drv.ver_res = 80;
    disp[1] = lv_disp_drv_register(&disp_drv);

    lv_disp_set_default(disp[0]);

    lv_task_create(memory_monitor, 3000, LV_TASK_PRIO_MID, NULL);

    gui_init(disp, lv_theme_material_init(0, NULL));

    while(1) {
        lv_task_handler();
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(gui_task, "gui_task", 4096, NULL, 5, NULL);
}
