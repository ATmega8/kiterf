#include "matrix.h"

/* res = A-1 */
int MATRIX_Inv(MATRIX_TypeDef *in, MATRIX_TypeDef *res)
{
    int32_t row, col, i;
    float tmp, tmp2;

    MATRIX_TypeDef A;

    A.rowCount = in->rowCount;
    A.colCount = in->colCount;

    float A_data[A.rowCount * A.rowCount];

    for (i = 0; i < A.rowCount * A.rowCount; i++) {
        A_data[i] = *(in->pdata + i);
    }

    A.pdata = (float *)&A_data;

    /*检查A是否为方阵*/
    if (A.rowCount != A.colCount) {
        return -1;
    }

    /* 初始化 res */
    for (row = 0; row < A.rowCount; row++) {
        for (col = 0; col < A.colCount; col++) {
            *(res->pdata + col + row * (A.rowCount)) = 0;
        }
    }

    /* 对角线元素为1 */
    for (row = 0; row < A.rowCount; row++) {
        *(res->pdata + row + row * (A.rowCount)) = 1;
    }

    /*左下清零*/
    for (col = 0; col < A.colCount - 1; col++) {
        for (row = col + 1; row < A.rowCount; row++) {
            tmp =     *(A.pdata + col + row * (A.rowCount)) / *(A.pdata + col + col * (A.colCount));

            for (i = col; i < A.colCount; i++) {

                tmp2 = (tmp * (*(A.pdata + col * (A.colCount) + i)));
                *(A.pdata + row * (A.colCount) + i) -=  tmp2;
            }

            for (i = 0; i < A.colCount; i++) {
                tmp2 = (tmp * (*(res->pdata + col * (res->colCount) + i)));
                *(res->pdata + row * (res->colCount) + i) -= tmp2;
            }
        }
    }

    /*右上清零*/
    for (col = A.rowCount - 1; col > 0; col--) {
        for (row = col - 1; row >= 0; row--) {
            tmp = (*(A.pdata + col + row * (A.rowCount)) / * (A.pdata + col + col * (A.colCount)));
            *(A.pdata + col + row * (A.colCount)) -=   tmp * (*(A.pdata + col * (A.colCount) + col));

            for (i = 0; i < A.colCount; i++) {
                *(res->pdata + row * (res->colCount) + i) -= tmp * (*(res->pdata + col * (res->colCount) + i));
            }
        }
    }

    /*检查矩阵是否奇异*/
    for (row = 0; row < A.rowCount; row++) {
        if ((*(A.pdata + row + row * (A.colCount)) > 0) && (*(A.pdata + row + row * (A.colCount)) < 1e-4)) {
            return -2;
        } else if ((*(A.pdata + row + row * (A.colCount)) < 0) && (*(A.pdata + row + row * (A.colCount)) > -1e-4)) {
            return -2;
        }

    }

    /*对角线归一化*/
    for (row = 0; row < A.rowCount; row++) {
        for (col = 0; col < A.colCount; col++) {
            *(res->pdata + col + row * (res->colCount)) /= *(A.pdata + row + row * (A.colCount));
        }
    }

    return 0;
}

