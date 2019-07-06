#include <stdint.h>

#pragma once

typedef struct
{
    uint16_t numTaps;     /**< number of filter coefficients in the filter. */
    double* pState;    /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
    double* pCoeffs;   /**< points to the coefficient array. The array is of length numTaps. */
} dsp_fir_instance_f64;

void dsp_fir_init_f64(dsp_fir_instance_f64* S,  uint16_t numTaps,  double* pCoeffs,  double* pState,  uint32_t blockSize);
void dsp_fir_f64(const dsp_fir_instance_f64 * S,  double * pSrc, double * pDst, uint32_t blockSize);