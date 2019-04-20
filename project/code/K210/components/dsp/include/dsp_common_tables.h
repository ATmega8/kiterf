#pragma once

#include "dsp_math.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint16_t dspBitRevTable[1024];
extern const float32_t twiddleCoef_16[32];
extern const float32_t twiddleCoef_32[64];
extern const float32_t twiddleCoef_64[128];
extern const float32_t twiddleCoef_128[256];
extern const float32_t twiddleCoef_256[512];
extern const float32_t twiddleCoef_512[1024];
extern const float32_t twiddleCoef_1024[2048];
extern const float32_t twiddleCoef_2048[4096];
extern const float32_t twiddleCoef_4096[8192];
#define twiddleCoef twiddleCoef_4096
extern const float32_t twiddleCoef_rfft_32[32];
extern const float32_t twiddleCoef_rfft_64[64];
extern const float32_t twiddleCoef_rfft_128[128];
extern const float32_t twiddleCoef_rfft_256[256];
extern const float32_t twiddleCoef_rfft_512[512];
extern const float32_t twiddleCoef_rfft_1024[1024];
extern const float32_t twiddleCoef_rfft_2048[2048];
extern const float32_t twiddleCoef_rfft_4096[4096];

/* floating-point bit reversal tables */
#define dspBITREVINDEXTABLE_16_TABLE_LENGTH ((uint16_t)20)
#define dspBITREVINDEXTABLE_32_TABLE_LENGTH ((uint16_t)48)
#define dspBITREVINDEXTABLE_64_TABLE_LENGTH ((uint16_t)56)
#define dspBITREVINDEXTABLE_128_TABLE_LENGTH ((uint16_t)208)
#define dspBITREVINDEXTABLE_256_TABLE_LENGTH ((uint16_t)440)
#define dspBITREVINDEXTABLE_512_TABLE_LENGTH ((uint16_t)448)
#define dspBITREVINDEXTABLE_1024_TABLE_LENGTH ((uint16_t)1800)
#define dspBITREVINDEXTABLE_2048_TABLE_LENGTH ((uint16_t)3808)
#define dspBITREVINDEXTABLE_4096_TABLE_LENGTH ((uint16_t)4032)

extern const uint16_t dspBitRevIndexTable16[dspBITREVINDEXTABLE_16_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable32[dspBITREVINDEXTABLE_32_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable64[dspBITREVINDEXTABLE_64_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable128[dspBITREVINDEXTABLE_128_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable256[dspBITREVINDEXTABLE_256_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable512[dspBITREVINDEXTABLE_512_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable1024[dspBITREVINDEXTABLE_1024_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable2048[dspBITREVINDEXTABLE_2048_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable4096[dspBITREVINDEXTABLE_4096_TABLE_LENGTH];

/* fixed-point bit reversal tables */
#define dspBITREVINDEXTABLE_FIXED_16_TABLE_LENGTH ((uint16_t)12)
#define dspBITREVINDEXTABLE_FIXED_32_TABLE_LENGTH ((uint16_t)24)
#define dspBITREVINDEXTABLE_FIXED_64_TABLE_LENGTH ((uint16_t)56)
#define dspBITREVINDEXTABLE_FIXED_128_TABLE_LENGTH ((uint16_t)112)
#define dspBITREVINDEXTABLE_FIXED_256_TABLE_LENGTH ((uint16_t)240)
#define dspBITREVINDEXTABLE_FIXED_512_TABLE_LENGTH ((uint16_t)480)
#define dspBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH ((uint16_t)992)
#define dspBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH ((uint16_t)1984)
#define dspBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH ((uint16_t)4032)

extern const uint16_t dspBitRevIndexTable_fixed_16[dspBITREVINDEXTABLE_FIXED_16_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_32[dspBITREVINDEXTABLE_FIXED_32_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_64[dspBITREVINDEXTABLE_FIXED_64_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_128[dspBITREVINDEXTABLE_FIXED_128_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_256[dspBITREVINDEXTABLE_FIXED_256_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_512[dspBITREVINDEXTABLE_FIXED_512_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_1024[dspBITREVINDEXTABLE_FIXED_1024_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_2048[dspBITREVINDEXTABLE_FIXED_2048_TABLE_LENGTH];
extern const uint16_t dspBitRevIndexTable_fixed_4096[dspBITREVINDEXTABLE_FIXED_4096_TABLE_LENGTH];

/* Tables for Fast Math Sine and Cosine */
extern const float32_t sinTable_f32[FAST_MATH_TABLE_SIZE + 1];

#ifdef __cplusplus
}
#endif
