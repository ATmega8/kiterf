#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fft_soft.h"

#define FFT_N               512U
#define FFT_FORWARD_SHIFT   0x0U
#define FFT_BACKWARD_SHIFT  0x1ffU
#define PI                  3.14159265358979323846

enum _complex_mode
{
    FFT_SOFT = 0,
    FFT_COMPLEX_MAX,
} ;

enum _fft_direction
{
    FFT_DIR_BACKWARD = 0,
    FFT_DIR_FORWARD,
    FFT_DIR_MAX,
} ;

enum _time_get
{
    TEST_START = 0,
    TEST_STOP = 1,
    TEST_TIME_MAX,
} ;

float soft_power[FFT_N];
float soft_angel[FFT_N];
struct timeval get_time[FFT_COMPLEX_MAX][FFT_DIR_MAX][TEST_TIME_MAX];

void fft_task(void *arg)
{
    int32_t i;
    float tempf1[3];
    complex data_soft[FFT_N] = {0};

    /* Data prepared for fft hard calculation and fft soft calculation. */
    for (i = 0; i < FFT_N; i++)
    {
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
    for (i = 0; i < FFT_N; i++)
    {
        soft_power[i] = sqrt(data_soft[i].real * data_soft[i].real + data_soft[i].imag * data_soft[i].imag) * 2;
    }
    printf("\nsoft power:\n");
    printf("%3d : %f\n", 0, soft_power[0] / 2 / FFT_N);
    for (i = 1; i < FFT_N / 2; i++)
        printf("%3d : %f\n", i, soft_power[i] / FFT_N);

    printf("\nsoft phase:\n");
    for (i = 0; i < FFT_N / 2; i++)
    {
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
            (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_sec -get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_usec),
            (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_sec -get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_usec));

    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void test_task(void *arg)
{
    uint32_t cnt = 0;
    while (1) {
        printf("hello kiterf! Count: %d\n", cnt++);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void app_main()
{
    xTaskCreate(fft_task, "fft_task", 10240, NULL, 5, NULL);
    xTaskCreate(test_task, "test_task", 2048, NULL, 5, NULL);
    while (1) {
        vTaskDelay(10 / portTICK_RATE_MS);
    }
}
