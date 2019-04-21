/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        dsp_cfft_init_f32.c
 * Description:  Split Radix Decimation in Frequency CFFT Floating point processing function
 *
 * $Date:        27. January 2017
 * $Revision:    V.1.5.1
 *
 * Target Processor: Cortex-M cores
 * -------------------------------------------------------------------- */
/*
 * Copyright (C) 2010-2017 dsp Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dsp_math.h"
#include "dsp_common_tables.h"

/**
 * @ingroup groupTransforms
 */

/**
 * @addtogroup RealFFT
 * @{
 */

/**
* @brief  Initialization function for the floating-point real FFT.
* @param[in,out] *S             points to an dsp_rfft_fast_instance_f32 structure.
* @param[in]     fftLen         length of the Real Sequence.
* @return        The function returns dsp_MATH_SUCCESS if initialization is successful or dsp_MATH_ARGUMENT_ERROR if <code>fftLen</code> is not a supported value.
*
* \par Description:
* \par
* The parameter <code>fftLen</code>	Specifies length of RFFT/CIFFT process. Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
* \par
* This Function also initializes Twiddle factor table pointer and Bit reversal table pointer.
*/
dsp_status dsp_rfft_fast_init_f32(
    dsp_rfft_fast_instance_f32 *S,
    uint16_t fftLen)
{
    dsp_cfft_instance_f32 *Sint;
    /*  Initialise the default dsp status */
    dsp_status status = dsp_MATH_SUCCESS;
    /*  Initialise the FFT length */
    Sint = &(S->Sint);
    Sint->fftLen = fftLen / 2;
    S->fftLenRFFT = fftLen;

    /*  Initializations of structure parameters depending on the FFT length */
    switch (Sint->fftLen) {
        case 2048U:
            /*  Initializations of structure parameters for 2048 point FFT */
            /*  Initialise the bit reversal table length */
            Sint->bitRevLength = dspBITREVINDEXTABLE_2048_TABLE_LENGTH;
            /*  Initialise the bit reversal table pointer */
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable2048;
            /*  Initialise the Twiddle coefficient pointers */
            Sint->pTwiddle     = (float32_t *) twiddleCoef_2048;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_4096;
            break;

        case 1024U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_1024_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable1024;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_1024;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_2048;
            break;

        case 512U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_512_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable512;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_512;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_1024;
            break;

        case 256U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_256_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable256;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_256;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_512;
            break;

        case 128U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_128_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable128;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_128;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_256;
            break;

        case 64U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_64_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable64;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_64;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_128;
            break;

        case 32U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_32_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable32;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_32;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_64;
            break;

        case 16U:
            Sint->bitRevLength = dspBITREVINDEXTABLE_16_TABLE_LENGTH;
            Sint->pBitRevTable = (uint16_t *)dspBitRevIndexTable16;
            Sint->pTwiddle     = (float32_t *) twiddleCoef_16;
            S->pTwiddleRFFT    = (float32_t *) twiddleCoef_rfft_32;
            break;

        default:
            /*  Reporting argument error if fftSize is not valid value */
            status = dsp_MATH_ARGUMENT_ERROR;
            break;
    }

    return (status);
}

/**
 * @} end of RealFFT group
 */
