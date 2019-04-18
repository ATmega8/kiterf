#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MATRIX_DATATYPE float

typedef struct
{
    uint32_t           rowCount;
    uint32_t           colCount;
    MATRIX_DATATYPE*   pdata;
} MATRIX_TypeDef;

typedef enum
{
    MatrixNoTrans,
    MatrixTrans
} MATRIX_TRANS_TypeDef;

MATRIX_TypeDef* MATRIX_Create(uint32_t row, uint32_t col);
void MATRIX_Destroy(MATRIX_TypeDef* matrix);

extern void MATRIX_SetData(MATRIX_TypeDef* matrix,
        const uint32_t row, const uint32_t col, const MATRIX_DATATYPE data);

extern MATRIX_DATATYPE MATRIX_GetData(const MATRIX_TypeDef* matrix,
        const uint32_t row, const uint32_t col);

void MATRIX_SetArrayData(MATRIX_TypeDef* matrix, void* data);

void MATRIX_Add(MATRIX_TypeDef* A, const MATRIX_TypeDef* B);
void MATRIX_Sub(MATRIX_TypeDef* A, const MATRIX_TypeDef* B);

void MATRIX_Mul(MATRIX_DATATYPE a, MATRIX_TRANS_TypeDef TA, MATRIX_TypeDef* A,
                    MATRIX_TRANS_TypeDef TB, MATRIX_TypeDef* B,
                            MATRIX_DATATYPE b, MATRIX_TypeDef* C);

int MATRIX_Inv(MATRIX_TypeDef* in, MATRIX_TypeDef* res);

#define MATRIX_ASSERT(expr) ((expr) ? (void)0 : MATRIX_AssertFailed(__FILE__, __LINE__))
void MATRIX_AssertFailed(const char* file, uint32_t line);

#ifdef __cplusplus
}
#endif
