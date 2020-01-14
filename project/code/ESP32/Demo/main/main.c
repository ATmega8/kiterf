#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
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

#define FFT_N               512U
#define FFT_FORWARD_SHIFT   0x0U
#define FFT_BACKWARD_SHIFT  0x1ffU
#define PI                  3.14159265358979323846

enum _complex_mode {
    FFT_SOFT = 0,
    FFT_COMPLEX_MAX,
} ;

enum _fft_direction {
    FFT_DIR_BACKWARD = 0,
    FFT_DIR_FORWARD,
    FFT_DIR_MAX,
} ;

enum _time_get {
    TEST_START = 0,
    TEST_STOP = 1,
    TEST_TIME_MAX,
} ;

float soft_power[FFT_N];
float soft_angel[FFT_N];
struct timeval get_time[FFT_COMPLEX_MAX][FFT_DIR_MAX][TEST_TIME_MAX];

SemaphoreHandle_t matrix_sem = NULL;
SemaphoreHandle_t dsp_sem = NULL;

void fft_task(void *arg)
{
    int32_t i;
    float tempf1[3];
    complex data_soft[FFT_N] = {0};

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    /* Data prepared for fft hard calculation and fft soft calculation. */
    for (i = 0; i < FFT_N; i++) {
        tempf1[0] = 0.3 * cosf(2 * PI * i / FFT_N + PI / 3) * 256;
        tempf1[1] = 0.1 * cosf(16 * 2 * PI * i / FFT_N - PI / 9) * 256;
        tempf1[2] = 0.5 * cosf((19 * 2 * PI * i / FFT_N) + PI / 6) * 256;
        data_soft[i].real = (int16_t)(tempf1[0] + tempf1[1] + tempf1[2] + 10);
        data_soft[i].imag = (int16_t)0;
    }

    /* FFT soft calculation. */
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START], NULL);
    fft_soft(data_soft, FFT_N);
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP], NULL);

    printf("\n[soft fft real][soft fft imag]\n");

    for (i = 0; i < FFT_N / 2; i++)
        printf("%3d:%7d %7d\n",
               i, (int32_t)data_soft[i].real, (int32_t)data_soft[i].imag);

    /* Power calculation. */
    for (i = 0; i < FFT_N; i++) {
        soft_power[i] = sqrt(data_soft[i].real * data_soft[i].real + data_soft[i].imag * data_soft[i].imag) * 2;
    }

    printf("\nsoft power:\n");
    printf("%3d : %f\n", 0, soft_power[0] / 2 / FFT_N);

    for (i = 1; i < FFT_N / 2; i++) {
        printf("%3d : %f\n", i, soft_power[i] / FFT_N);
    }

    printf("\nsoft phase:\n");

    for (i = 0; i < FFT_N / 2; i++) {
        soft_angel[i] = atan2(data_soft[i].imag, data_soft[i].real);
        printf("%3d : %f\n", i, soft_angel[i] * 180 / PI);
    }

    /* IFFT soft calculation. */
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START], NULL);
    ifft_soft(data_soft, FFT_N);
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP], NULL);

    printf("\n[soft ifft real][soft ifft imag]\n");

    for (i = 0; i < FFT_N / 2; i++)
        printf("%3d:%7d %7d\n",
               i, (int32_t)data_soft[i].real, (int32_t)data_soft[i].imag);

    printf("[soft ][%d bytes][forward time = %ld us][backward time = %ld us]\n",
           FFT_N,
           (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_sec - get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_sec) * 1000 * 1000 +
           (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_usec),
           (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_sec - get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_sec) * 1000 * 1000 +
           (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_usec));
    xSemaphoreGive(matrix_sem);

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

    /*Create a Calendar object*/
    lv_obj_t * calendar = lv_calendar_create(lv_scr_act(), NULL);
    lv_obj_set_size(calendar, 240, 220);
    lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Create a style for the current week*/
    static lv_style_t style_week_box;
    lv_style_copy(&style_week_box, &lv_style_plain);
    style_week_box.body.border.width = 1;
    style_week_box.body.border.color = LV_COLOR_HEX3(0x333);
    style_week_box.body.empty = 1;
    style_week_box.body.radius = LV_RADIUS_CIRCLE;
    style_week_box.body.padding.ver = 3;
    style_week_box.body.padding.hor = 3;

    /*Create a style for today*/
    static lv_style_t style_today_box;
    lv_style_copy(&style_today_box, &lv_style_plain);
    style_today_box.body.border.width = 2;
    style_today_box.body.border.color = LV_COLOR_NAVY;
    style_today_box.body.empty = 1;
    style_today_box.body.radius = LV_RADIUS_CIRCLE;
    style_today_box.body.padding.ver = 3;
    style_today_box.body.padding.hor = 3;
    style_today_box.text.color= LV_COLOR_BLUE;

    /*Create a style for the highlighted days*/
    static lv_style_t style_highlighted_day;
    lv_style_copy(&style_highlighted_day, &lv_style_plain);
    style_highlighted_day.body.border.width = 2;
    style_highlighted_day.body.border.color = LV_COLOR_NAVY;
    style_highlighted_day.body.empty = 1;
    style_highlighted_day.body.radius = LV_RADIUS_CIRCLE;
    style_highlighted_day.body.padding.ver = 3;
    style_highlighted_day.body.padding.hor = 3;
    style_highlighted_day.text.color= LV_COLOR_BLUE;

    /*Apply the styles*/
    lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_WEEK_BOX, &style_week_box);
    lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_TODAY_BOX, &style_today_box);
    lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS, &style_highlighted_day);


    /*Set the today*/
    lv_calendar_date_t today;
    today.year = 2018;
    today.month = 10;
    today.day = 23;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);

    /*Highlight some days*/
    static lv_calendar_date_t highlihted_days[3];       /*Only it's pointer will be saved so should be static*/
    highlihted_days[0].year = 2018;
    highlihted_days[0].month = 10;
    highlihted_days[0].day = 6;

    highlihted_days[1].year = 2018;
    highlihted_days[1].month = 10;
    highlihted_days[1].day = 11;

    highlihted_days[2].year = 2018;
    highlihted_days[2].month = 11;
    highlihted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(calendar, highlihted_days, 3);

    while (1) {
        lv_task_handler();
        vTaskDelay(10 / portTICK_RATE_MS);
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

    xTaskCreate(fft_task, "fft_task", 2560 * 4, NULL, 5, NULL);
    xTaskCreate(matrix_task, "matrix_task", 512 * 4, NULL, 5, NULL);
    xTaskCreate(dsp_task, "dsp_task", 1024 * 4, NULL, 5, NULL);
    xTaskCreate(lvgl_task, "lvgl_task", 1024 * 4, NULL, 5, NULL);

    vTaskDelay(5000 / portTICK_RATE_MS);

    const char *ESP_LUA_ARGV[2] = {"./lua", NULL};
    esp_lua_init(NULL, NULL, NULL);
    while (1) {
        esp_lua_main(1, ESP_LUA_ARGV);
    }

    vTaskSuspend(NULL);
}
