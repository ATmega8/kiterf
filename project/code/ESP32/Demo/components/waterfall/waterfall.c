//
// Created by life on 20-3-5.
//

#include "lvgl.h"
#include "esp_dsp.h"

#include "iq_data.h"

#include <math.h>

#define FFT_N               512U
#define PLOT_WIDTH          480
#define PLOT_HEIGHT         160

float fft_data[2*FFT_N];

//TODO: const table
float fft_win[FFT_N];

int32_t tempBuf[FFT_N];
uint8_t  waterfall_line[PLOT_WIDTH];

extern lv_img_dsc_t waterfall;

static void fftDataToInteger(uint32_t width, uint32_t height,
                      float maxdB, float mindB,
                      float *in, uint8_t *out)
{
    int32_t ymax = 10000;
    int32_t xprev = -1;
    float  dBGainFactor = ((float)height) / fabsf(maxdB - mindB);

    for (int i = 0; i < FFT_N; i++) {
        tempBuf[i] = ((int64_t)(i) * width) /FFT_N;
    }

    for (int i = 0; i < FFT_N; i++) {
        int32_t y = (int32_t)(dBGainFactor * (maxdB - in[i]));

        if (y > height) {
            y = height;
        } else if (y < 0) {
            y = 0;
        }

        int32_t x = tempBuf[i];	//get fft bin to plot x coordinate transform

        if (x == xprev) { // still mappped to same fft bin coordinate
            if(y < ymax) {
                out[x] = y;
                ymax = y;
            }
        } else {
            xprev = x;
            out[x] = y;
            ymax = y;
        }
    }
}

static void waterfall_scroll(uint32_t height, uint32_t width, uint8_t *data)
{
    for (int i = 0; i < height - 1; ++i) {
        memcpy(&data[i * width], &data[(i + 1)*width], width);
    }
}

void lvgl_update_waterfall(lv_obj_t* waterfall_image)
{
    float i_data, q_data;
    float* iptr = &fft_data[0];
    float* qptr = &fft_data[FFT_N];

    /* Get IQ data */
    dsps_wind_hann_f32(&fft_win, FFT_N);

    /* Data prepared for fft hard calculation and fft soft calculation. */
    for (int i = 0; i < FFT_N; i++) {
        memcpy(&i_data, &iq_98608_1_bin[i*8], 4);
        memcpy(&q_data, &iq_98608_1_bin[i*8+4], 4);

        iptr[i] = i_data*fft_win[i];
        qptr[i] = q_data*fft_win[i];
    }

    /* do fft */
    dsps_cplx2reC_fc32(fft_data, FFT_N);

    for (int i = 0; i < FFT_N; ++i) {
        fft_data[i] = 20 * log10f(sqrtf((iptr[i] *iptr[i]) + (qptr[i] * qptr[i])));
    }

    fftDataToInteger(PLOT_WIDTH, 255, 40, -10, iptr, waterfall_line);

    /* update image */
    uint8_t *databuf = waterfall.data;

    waterfall_scroll(PLOT_HEIGHT, PLOT_WIDTH, &databuf[1024]);

    for (int j = 0; j < PLOT_WIDTH; ++j) {
        databuf[1024 + (PLOT_HEIGHT-1)*PLOT_WIDTH+ j] =  255 - waterfall_line[j];
    }

    lv_img_set_src(waterfall_image, &waterfall);
}

