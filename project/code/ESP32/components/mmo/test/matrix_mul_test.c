#include "matrix.h"
#include "unity.h"

/* C = a*A*B + b*C*/
/* a = 1.0 */
/* b = 0.0 */

//#define UNITY_FLOAT_PRECISION (0.00001f)

void testMatrixMulNoTransNoAdd(void)
{
    MATRIX_DATATYPE AData [12] = {0.11885142, 0.56248665, 0.43614328,
                                  0.9335573, 0.37207317, 0.7278658,
                                  0.45821178, 0.5312071, 0.57842934,
                                  0.4567381, 0.5078081, 0.6672957
                                 };

    MATRIX_DATATYPE BData [12] = {0.51302075, 0.2695328, 0.78012717, 0.93048406,
                                  0.07831228, 0.26152325, 0.88859534, 0.80999017,
                                  0.8040291, 0.112962365, 0.51138175, 0.023115635
                                 };

    MATRIX_DATATYPE RData [16] = {0.45569474, 0.22840548, 0.8155779, 0.5762797,
                                  1.0932975, 0.43115154, 1.4311332, 1.1868608,
                                  0.7417462, 0.32776687, 1.1252898, 0.87000203,
                                  0.8106089, 0.33128884, 1.1487925, 0.8517321
                                 };

    MATRIX_TypeDef *A = MATRIX_Create(4, 3);
    MATRIX_TypeDef *B = MATRIX_Create(3, 4);
    MATRIX_TypeDef *C = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);
    MATRIX_SetArrayData(B, &BData[0]);

    MATRIX_Mul(1.0,
               MatrixNoTrans, A,
               MatrixNoTrans, B,
               0.0, C);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(C->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
    MATRIX_Destroy(C);
}

/* C = a*A*B + b*C*/
/* a = 1.0 */
/* b = 1.0 */

void testMatrixMulNoTransAdd(void)
{
    MATRIX_DATATYPE AData [12] = {0.11885142, 0.56248665, 0.43614328,
                                  0.9335573, 0.37207317, 0.7278658,
                                  0.45821178, 0.5312071, 0.57842934,
                                  0.4567381, 0.5078081, 0.6672957
                                 };

    MATRIX_DATATYPE BData [12] = {0.51302075, 0.2695328, 0.78012717, 0.93048406,
                                  0.07831228, 0.26152325, 0.88859534, 0.80999017,
                                  0.8040291, 0.112962365, 0.51138175, 0.023115635
                                 };

    MATRIX_DATATYPE CData [16] = {0.144704,  0.262143, 0.510371,  0.952889,
                                  0.985299,  0.242126, 0.326276,  0.046031,
                                  0.0221534, 0.0252633, 0.943077,  0.488633,
                                  0.697973,  0.785507, 0.870777,  0.486606
                                 };

    MATRIX_DATATYPE RData [16] = {0.60039884, 0.49054813, 1.325949, 1.5291688,
                                  2.0785966, 0.67327726, 1.7574087, 1.2328918,
                                  0.76389956, 0.35303017, 2.0683665, 1.3586351,
                                  1.5085824, 1.1167957, 2.0195699, 1.3383381
                                 };

    MATRIX_TypeDef *A = MATRIX_Create(4, 3);
    MATRIX_TypeDef *B = MATRIX_Create(3, 4);
    MATRIX_TypeDef *C = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);
    MATRIX_SetArrayData(B, &BData[0]);
    MATRIX_SetArrayData(C, &CData[0]);

    MATRIX_Mul(1.0,
               MatrixNoTrans, A,
               MatrixNoTrans, B,
               1.0, C);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(C->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
    MATRIX_Destroy(C);
}

void testMatrixMulATransAdd(void)
{
    MATRIX_DATATYPE BData [12] = {0.51302075, 0.2695328, 0.78012717, 0.93048406,
                                  0.07831228, 0.26152325, 0.88859534, 0.80999017,
                                  0.8040291, 0.112962365, 0.51138175, 0.023115635
                                 };

    MATRIX_DATATYPE CData [16] = {0.144704,  0.262143, 0.510371,  0.952889,
                                  0.985299,  0.242126, 0.326276,  0.046031,
                                  0.0221534, 0.0252633, 0.943077,  0.488633,
                                  0.697973,  0.785507, 0.870777,  0.486606
                                 };

    MATRIX_DATATYPE RData [16] = {1.06049, 0.5117241, 1.3913462, 1.5122645,
                                  1.2348807, 0.39592856, 0.8267007, 0.51126945,
                                  0.90312856, 0.5256884, 2.602788, 1.9461033,
                                  1.2573489, 1.2507453, 2.3282475, 2.009025
                                 };

    MATRIX_TypeDef *A = MATRIX_Create(3, 4);
    MATRIX_TypeDef *B = MATRIX_Create(3, 4);
    MATRIX_TypeDef *C = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &BData[0]);
    MATRIX_SetArrayData(B, &BData[0]);
    MATRIX_SetArrayData(C, &CData[0]);

    MATRIX_Mul(1.0,
               MatrixTrans, A,
               MatrixNoTrans, B,
               1.0, C);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(C->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
    MATRIX_Destroy(C);
}

void testMatrixMulBTransAdd(void)
{
    MATRIX_DATATYPE AData [12] = {0.11885142, 0.56248665, 0.43614328,
                                  0.9335573, 0.37207317, 0.7278658,
                                  0.45821178, 0.5312071, 0.57842934,
                                  0.4567381, 0.5078081, 0.6672957
                                 };

    MATRIX_DATATYPE CData [16] = {0.144704,  0.262143, 0.510371, 0.952889,
                                  0.985299,  0.242126, 0.326276, 0.046031,
                                  0.0221534, 0.0252633, 0.943077, 0.488633,
                                  0.697973,  0.785507, 0.870777, 0.486606
                                 };

    MATRIX_DATATYPE RData [16] = {0.665442, 0.89983726, 1.1159052, 1.5838449,
                                  1.6229938, 1.7818819, 1.3727094, 1.1470656,
                                  0.62768745, 1.0716971, 1.7697961, 1.3536506,
                                  1.3289292, 1.8865415, 1.7357948, 1.3983684
                                 };

    MATRIX_TypeDef *A = MATRIX_Create(4, 3);
    MATRIX_TypeDef *B = MATRIX_Create(4, 3);
    MATRIX_TypeDef *C = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);
    MATRIX_SetArrayData(B, &AData[0]);
    MATRIX_SetArrayData(C, &CData[0]);

    MATRIX_Mul(1.0,
               MatrixNoTrans, A,
               MatrixTrans,   B,
               1.0, C);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(C->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
    MATRIX_Destroy(C);

}

void RunMatrixMulTest(void)
{
    RUN_TEST(testMatrixMulNoTransNoAdd, 4);
    RUN_TEST(testMatrixMulNoTransAdd, 48);
    RUN_TEST(testMatrixMulATransAdd, 90);
    RUN_TEST(testMatrixMulBTransAdd, 126);
}
