#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <tca9555.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "fft_soft.h"
#include "matrix.h"
#include "test.h"
#include "unity.h"
#include "dsp_math.h"
#include "lvgl.h"
#include "lcd.h"
#include "esp_lua.h"
#include "esp_dsp.h"

#include "waterfall.h"
#include "msi001.h"
#include "rfswitch.h"
#include "cs5361.h"

#define FFT_FORWARD_SHIFT   0x0U
#define FFT_BACKWARD_SHIFT  0x1ffU
#define PI                  3.14159265358979323846

extern lv_img_dsc_t waterfall;

SemaphoreHandle_t matrix_sem = NULL;
SemaphoreHandle_t dsp_sem = NULL;

void fft_task(void *arg)
{
    vTaskSuspend(NULL);
}

void matrix_task(void *arg)
{
    xSemaphoreTake(matrix_sem, portMAX_DELAY);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    UnityBegin("matrix test");
    RunMatrixTest();
    RunMatrixAddTest();
    RunMatrixSubTest();
    RunMatrixMulTest();
    RunMatrixInvTest();
    UnityEnd();
    xSemaphoreGive(dsp_sem);

    vTaskSuspend(NULL);
}

static float cof[38] = {-0.00028361, -0.00075393, -0.00109268, -0.0006769, 0.00102914, 0.00391172, \
                        0.00678037, 0.0074884, 0.00382395, -0.00503141, -0.0171363, -0.02737452, \
                        -0.02859017, -0.01418401, 0.01873238, 0.0670341, 0.12139937, 0.16869521, \
                        0.19622892, 0.19622892, 0.16869521, 0.12139937, 0.0670341, 0.01873238, \
                        -0.01418401, -0.02859017, -0.0273745, -0.0171363, -0.00503141, 0.00382395, \
                        0.0074884, 0.00678037, 0.00391172, 0.00102914, -0.0006769, -0.00109268, \
                        -0.00075393, -0.00028361
                        };

static float x_data[64] = {1.1016856340840009, 1.3819116564898124, 1.2902960763808005, 1.0566626343732346,
                           0.9442254510513154, 0.9579868821695956, 1.046306253339488, 1.2410366852938386,
                           1.4436282176750759, 1.3874515506551908, 1.0378805602935988, 0.7756155443385301,
                           0.9178050511383941, 1.2537702552749763, 1.3426204627941956, 1.117610065929717,
                           0.8800604905211858, 0.8012517778630224, 0.7821389128510301, 0.7711632039505969,
                           0.8501510692896972, 0.951564312477108, 0.8436661347816815, 0.5268377902750552,
                           0.33441324855963317, 0.4608704807237677, 0.660838911589431, 0.6315802275119023,
                           0.4503064294285132, 0.3690546542314113, 0.3830365542353578, 0.32286699253190193,
                           0.23477111445056748, 0.30990336758188164, 0.4885556520187167, 0.49487230610275945,
                           0.29017088466245566, 0.15479252877059746, 0.24169920785325388, 0.3670607943691402,
                           0.36701157780354643, 0.3520189111198753, 0.42271529556318366, 0.4225052367712395,
                           0.23092439222513883, 0.06926710273608785, 0.1994021343386783, 0.4723172130423151,
                           0.5111771910438687, 0.2622959422441363, 0.03179207010391415, 0.00435686084990973,
                           0.060746245121383116, 0.09208186182560787, 0.14769880330463428, 0.18278609616995745,
                           0.004175541124054452, -0.36016119316009115, -0.5430364808518742, -0.33032288155563755,
                           -0.031070289589745434, -0.0789812885502211, -0.444599870929681, -0.7399276026332652
                           };

void dsp_task(void *arg)
{
    int i;
    float res[64] = {0.0};
    float state[64 + 37] = {0.0};
    float mag[33] = {0};
    float x[2] = {0.333, 1.2};
    float y[2] = {32.1, 1000.2};

    xSemaphoreTake(dsp_sem, portMAX_DELAY);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    // add and mul
    dsp_add_f32(x, y, res, 2);
    printf("%f + %f = %f\n", x[0], y[0], res[0]);
    printf("%f + %f = %f\n", x[1], y[1], res[1]);

    // sin and cos
    res[0] = dsp_sin_f32(0.1);
    printf("sin(%f) = %f\n", 0.1, res[0]);
    res[0] = dsp_cos_f32(2.0);
    printf("cos(%f) = %f\n", 2.0, res[0]);

    // fir
    dsp_fir_instance_f32 fir;
    dsp_fir_init_f32(&fir, 38, cof, state, 64);
    dsp_fir_f32(&fir, x_data, res, 64);
    printf("FIR Result:\n");

    for (i = 0; i < 64; i = i + 4) {
        printf("[%f, %f, %f, %f]\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
    }

    // rfft
    dsp_rfft_fast_instance_f32 rfft;
    dsp_rfft_fast_init_f32(&rfft, 64);
    dsp_rfft_fast_f32(&rfft, x_data, res, 0);
    printf("RFFT Result:\n");

    for (i = 0; i < 64; i = i + 4) {
        printf("[%f, %f, %f, %f]\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
    }

    dsp_cmplx_mag_f32(res, mag, 64);
    mag[0] = res[0];
    mag[32] = res[1];
    printf("RFFT Magnitude Result:\n");

    for (i = 0; i < 32; i = i + 4) {
        printf("[%f, %f, %f, %f]\n", mag[i], mag[i + 1], mag[i + 2], mag[i + 3]);
    }

    printf("[%f]\n", mag[32]);

    dsp_rfft_fast_instance_f32 rifft;
    dsp_rfft_fast_init_f32(&rifft, 64);
    float ifft_res[64];
    dsp_rfft_fast_f32(&rifft, res, ifft_res, 1);
    printf("IRFFT Result:\n");

    for (i = 0; i < 64; i = i + 4) {
        printf("[%f, %f, %f, %f]\n", ifft_res[i], ifft_res[i + 1], ifft_res[i + 2], ifft_res[i + 3]);
    }
    printf("heap: %d\n", esp_get_free_heap_size());
    vTaskSuspend(NULL);
}

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

void app_main()
{
    matrix_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(matrix_sem, 0);
    dsp_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(dsp_sem, 0);

    lv_init();
    lcd_init();
    tac9555_i2c_init();
    vTaskDelay(100);
    tca9555_init();
    msi001_standby();
    rf_switch_set_mode(RF_SWITCH_VHF1_MODE);
    cs5361_init();

    //xTaskCreate(fft_task, "fft_task", 2560 * 4, NULL, 5, NULL);
    //xTaskCreate(matrix_task, "matrix_task", 512 * 4, NULL, 5, NULL);
    //xTaskCreate(dsp_task, "dsp_task", 1024 * 4, NULL, 5, NULL);
    xTaskCreate(lvgl_task, "lvgl_task", 1024 * 4, NULL, 5, NULL);

    vTaskDelay(5000 / portTICK_RATE_MS);

    //const char *ESP_LUA_ARGV[2] = {"./lua", NULL};
    //esp_lua_init(NULL, NULL, NULL);

    while (1) {
        //esp_lua_main(1, ESP_LUA_ARGV);
    }

    vTaskSuspend(NULL);
}
