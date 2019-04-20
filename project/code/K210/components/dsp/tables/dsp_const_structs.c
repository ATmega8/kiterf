
#include "dsp_const_structs.h"

/* Floating-point structs */
const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len16 = {
    16, twiddleCoef_16, dspBitRevIndexTable16, dspBITREVINDEXTABLE_16_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len32 = {
    32, twiddleCoef_32, dspBitRevIndexTable32, dspBITREVINDEXTABLE_32_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len64 = {
    64, twiddleCoef_64, dspBitRevIndexTable64, dspBITREVINDEXTABLE_64_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len128 = {
    128, twiddleCoef_128, dspBitRevIndexTable128, dspBITREVINDEXTABLE_128_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len256 = {
    256, twiddleCoef_256, dspBitRevIndexTable256, dspBITREVINDEXTABLE_256_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len512 = {
    512, twiddleCoef_512, dspBitRevIndexTable512, dspBITREVINDEXTABLE_512_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len1024 = {
    1024, twiddleCoef_1024, dspBitRevIndexTable1024, dspBITREVINDEXTABLE_1024_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len2048 = {
    2048, twiddleCoef_2048, dspBitRevIndexTable2048, dspBITREVINDEXTABLE_2048_TABLE_LENGTH
};

const dsp_cfft_instance_f32 dsp_cfft_sR_f32_len4096 = {
    4096, twiddleCoef_4096, dspBitRevIndexTable4096, dspBITREVINDEXTABLE_4096_TABLE_LENGTH
};



/* Structure for real-value inputs */
/* Floating-point structs */
const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len32 = {
    { 16, twiddleCoef_32, dspBitRevIndexTable32, dspBITREVINDEXTABLE_16_TABLE_LENGTH },
    32U,
    (float32_t *)twiddleCoef_rfft_32
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len64 = {
    { 32, twiddleCoef_32, dspBitRevIndexTable32, dspBITREVINDEXTABLE_32_TABLE_LENGTH },
    64U,
    (float32_t *)twiddleCoef_rfft_64
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len128 = {
    { 64, twiddleCoef_64, dspBitRevIndexTable64, dspBITREVINDEXTABLE_64_TABLE_LENGTH },
    128U,
    (float32_t *)twiddleCoef_rfft_128
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len256 = {
    { 128, twiddleCoef_128, dspBitRevIndexTable128, dspBITREVINDEXTABLE_128_TABLE_LENGTH },
    256U,
    (float32_t *)twiddleCoef_rfft_256
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len512 = {
    { 256, twiddleCoef_256, dspBitRevIndexTable256, dspBITREVINDEXTABLE_256_TABLE_LENGTH },
    512U,
    (float32_t *)twiddleCoef_rfft_512
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len1024 = {
    { 512, twiddleCoef_512, dspBitRevIndexTable512, dspBITREVINDEXTABLE_512_TABLE_LENGTH },
    1024U,
    (float32_t *)twiddleCoef_rfft_1024
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len2048 = {
    { 1024, twiddleCoef_1024, dspBitRevIndexTable1024, dspBITREVINDEXTABLE_1024_TABLE_LENGTH },
    2048U,
    (float32_t *)twiddleCoef_rfft_2048
};

const dsp_rfft_fast_instance_f32 dsp_rfft_fast_sR_f32_len4096 = {
    { 2048, twiddleCoef_2048, dspBitRevIndexTable2048, dspBITREVINDEXTABLE_2048_TABLE_LENGTH },
    4096U,
    (float32_t *)twiddleCoef_rfft_4096
};
