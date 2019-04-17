#include "matrix.h"
#include "unity.h"

void testMatrixCreate(void)
{
    MATRIX_TypeDef* A = MATRIX_Create(4, 4);

    TEST_ASSERT_NOT_NULL(A);
    TEST_ASSERT_NOT_NULL(A->pdata);

    MATRIX_Destroy(A);
}

void testMatrixSetAndGet(void)
{
    MATRIX_DATATYPE setData, getData;

    MATRIX_TypeDef* A = MATRIX_Create(4, 4);

    setData = 1.31;

    MATRIX_SetData(A, 1, 2, setData);
    getData = MATRIX_GetData(A, 1, 2);

    TEST_ASSERT_EQUAL_FLOAT(setData, getData);

    MATRIX_SetData(A, 2, 1, setData);
    getData = MATRIX_GetData(A, 2, 1);

    TEST_ASSERT_EQUAL_FLOAT(setData, getData);

    MATRIX_Destroy(A);
}

void RunMatrixTest(void)
{
    RUN_TEST(testMatrixCreate, 4);
    RUN_TEST(testMatrixSetAndGet, 14);
}

