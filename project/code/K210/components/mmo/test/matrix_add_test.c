#include "matrix.h"
#include "unity.h"

void testMatrixAdd(void)
{
    MATRIX_DATATYPE AData [16] = {0.8954921, 0.2663976, 0.21790004, 0.23620462,
                                   0.16729498, 0.73271763, 0.51829493, 0.45169485,
                                  0.8152236, 0.31864083, 0.7929952, 0.15479577,
                                  0.23447287, 0.22127843, 0.9477545, 0.9974389};

    MATRIX_DATATYPE BData [16] = {0.8560777, 0.9051882, 0.4354645, 0.11091411,
                                  0.2586733, 0.18896914, 0.5719185, 0.17071259,
                                  0.5473523, 0.29790926, 0.9116328, 0.2195952,
                                  0.41048408, 0.85772, 0.8150879, 0.7476915};

    MATRIX_DATATYPE RData[16] = {1.7515697, 1.1715858, 0.65336454, 0.34711874,
                                 0.4259683, 0.92168677, 1.0902134, 0.62240744,
                                 1.3625759, 0.6165501, 1.704628, 0.37439096,
                                 0.64495695, 1.0789984, 1.7628424, 1.7451304};

    MATRIX_TypeDef* A = MATRIX_Create(4, 4);
    MATRIX_TypeDef* B = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);
    MATRIX_SetArrayData(B, &BData[0]);

    MATRIX_Add(A, B);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(A->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
}

void RunMatrixAddTest(void)
{
    RUN_TEST(testMatrixAdd, 4);
}

