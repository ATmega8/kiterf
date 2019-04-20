#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "matrix.h"

MATRIX_TypeDef *MATRIX_Create(const uint32_t row, uint32_t const col)
{
    MATRIX_TypeDef *matrix = (MATRIX_TypeDef *)malloc(sizeof(MATRIX_TypeDef));

    matrix->rowCount = row;
    matrix->colCount = col;
    matrix->pdata = (MATRIX_DATATYPE *)malloc(sizeof(MATRIX_DATATYPE) * row * col);

    return matrix;
}

void MATRIX_Destroy(MATRIX_TypeDef *matrix)
{
    free(matrix->pdata);
    free(matrix);
}

inline void MATRIX_SetData(MATRIX_TypeDef *matrix,
                           const uint32_t row, const uint32_t col, const MATRIX_DATATYPE data)
{
    MATRIX_ASSERT(matrix != NULL);
    MATRIX_ASSERT((col >= 0) && (col < matrix->colCount));
    MATRIX_ASSERT((col >= 0) && (col < matrix->colCount));

    *(matrix->pdata    + matrix->colCount * row + col) = data;
}

inline MATRIX_DATATYPE MATRIX_GetData(const MATRIX_TypeDef *matrix,
                                      const uint32_t row, const uint32_t col)
{
    MATRIX_ASSERT(matrix != NULL);
    MATRIX_ASSERT((col >= 0) && (col < matrix->colCount));
    MATRIX_ASSERT((col >= 0) && (col < matrix->colCount));

    return *(matrix->pdata    + matrix->colCount * row + col);
}

void MATRIX_SetArrayData(MATRIX_TypeDef *matrix, void *data)
{
    MATRIX_ASSERT(matrix != NULL);

    memcpy(matrix->pdata, data, sizeof(MATRIX_DATATYPE)
           * (matrix->rowCount) * (matrix->colCount));
}

void MATRIX_AssertFailed(const char *file, uint32_t line)
{
    printf("Matrix Assert Failed, File: %s, Line: %d\n", file, line);

    exit(-1);
}

