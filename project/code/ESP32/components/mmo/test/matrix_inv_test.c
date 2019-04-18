#include "matrix.h"
#include "unity.h"

void testMatrixInv(void)
{
    MATRIX_DATATYPE AData[16] = {0.5671705, 0.85857487, 0.24772811, 0.17200792,
                                    0.33526444, 0.6074699, 0.38616037, 0.48290718,
                                 0.64895415, 0.98818684, 0.072078705, 0.49692154,
                                 0.04134202, 0.3594458, 0.78464615, 0.21125221};

    MATRIX_DATATYPE RData[16] = { 11.088349, 18.099255, -16.808384, -10.86424,
                                 -6.673499, -13.947189, 12.539795, 7.8190827,
                                  2.9123397, 4.508056, -4.783549, -1.4242203,
                                 -1.6322169, 3.4450305, -0.27971768, -1.1544396};

    MATRIX_TypeDef* A = MATRIX_Create(4, 4);
    MATRIX_TypeDef* B = MATRIX_Create(4, 4);

    MATRIX_SetArrayData(A, &AData[0]);

    MATRIX_Inv(A, B);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(B->pdata, RData, 16);

    MATRIX_Destroy(A);
    MATRIX_Destroy(B);
}

void RunMatrixInvTest(void)
{
    RUN_TEST(testMatrixInv, 4);
}

