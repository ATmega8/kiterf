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

    vTaskSuspend(NULL);
}

void app_main()
{
    matrix_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(matrix_sem, 0);
    dsp_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(dsp_sem, 0);

    xTaskCreate(fft_task, "fft_task", 2560 * 4, NULL, 5, NULL);
    xTaskCreate(matrix_task, "matrix_task", 512 * 4, NULL, 5, NULL);
    xTaskCreate(dsp_task, "dsp_task", 1024 * 4, NULL, 5, NULL);

    vTaskSuspend(NULL);
}
