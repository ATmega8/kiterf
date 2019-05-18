#include "matrix.h"
#include "unity.h"

void testMatrixSub(void)
{
    MATRIX_DATATYPE AData [16] = {0.8954921, 0.2663976, 0.21790004, 0.23620462,
                                  0.16729498, 0.73271763, 0.51829493, 0.45169485,
                                  0.8152236, 0.31864083, 0.7929952, 0.15479577,
                                  0.23447287, 0.22127843, 0.9477545, 0.9974389
                                 };

    MATRIX_DATATYPE BData [16] = {0.8560777, 0.9051882, 0.4354645, 0.11091411,
                                  0.2586733, 0.18896914, 0.5719185, 0.17071259,
                                  0.5473523, 0.29790926, 0.9116328, 0.2195952,
                                  0.41048408, 0.85772, 0.8150879, 0.7476915
                                 };

    MATRIX_DATATYPE RData[16] = { 0.039414406, -0.6387906, -0.21756446, 0.12529051,
                                  -0.09137833, 0.5437485, -0.053623557, 0.28098226,
                                  0.26787126, 0.020731568, -0.11863756, -0.06479943,
                                  -0.1760112, -0.6364416, 0.13266659, 0.2497474
                                };

    MATRIX_TypeDef *A = MATRIX_Create(4, 4);
    MATRIX_TypeDef *B = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);
    MATRIX_SetArrayData(B, &BData[0]);

    MATRIX_Sub(A, B);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(A->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
}

void RunMatrixSubTest(void)
{
    RUN_TEST(testMatrixSub, 4);
}

