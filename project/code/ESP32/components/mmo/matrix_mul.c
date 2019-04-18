#include "matrix.h"

/*C = a*op(A)*op(B) + b*C*/
void MATRIX_Mul(MATRIX_DATATYPE a, MATRIX_TRANS_TypeDef TA, MATRIX_TypeDef* A, 
                    MATRIX_TRANS_TypeDef TB, MATRIX_TypeDef* B,
                            MATRIX_DATATYPE b, MATRIX_TypeDef* C)
{
    uint32_t row, col, i;
    MATRIX_DATATYPE sum = 0;

    /*相乘*/
    if((TA == MatrixNoTrans) && (TB == MatrixNoTrans))
    {
        MATRIX_ASSERT(A->colCount == B->rowCount);
        MATRIX_ASSERT(C->colCount == B->colCount);
        MATRIX_ASSERT(C->rowCount == A->rowCount);

        for(row = 0; row < A->rowCount; row++)
        {
            for(col = 0; col < B->colCount; col++)    
            {
                for(i = 0; i < A->colCount; i++)
                {
                        sum += 
                            (*(A->pdata + row*(A->colCount) + i)) * 
                            (*(B->pdata + i*(B->colCount) + col));
                }

                *(C->pdata + col + row*(B->colCount)) *= b;
                *(C->pdata + col + row*(B->colCount)) += sum*a;

                sum = 0.0;
            }
        }
    }
    else if((TA == MatrixTrans) && (TB == MatrixNoTrans))
    {
        MATRIX_ASSERT(A->rowCount == B->rowCount);
        MATRIX_ASSERT(C->colCount == B->colCount);
        MATRIX_ASSERT(C->rowCount == A->colCount);

        for(row = 0; row < A->colCount; row++)
        {
            for(col = 0; col < B->colCount; col++)    
            {
                for(i = 0; i < A->rowCount; i++)
                {
                        sum += 
                            (*(A->pdata + i*(A->colCount) + row)) * 
                            (*(B->pdata + i*(B->colCount) + col));
                }

                *(C->pdata + col + row*(B->colCount)) *= b;
                *(C->pdata + col + row*(B->colCount)) += sum*a;

                sum = 0.0;
            }
        }
    }
    else if((TA == MatrixNoTrans) && (TB == MatrixTrans))
    {
        MATRIX_ASSERT(A->colCount == B->colCount);
        MATRIX_ASSERT(C->colCount == B->rowCount);
        MATRIX_ASSERT(C->rowCount == A->rowCount);

        for(row = 0; row < A->rowCount; row++)
        {
            for(col = 0; col < B->rowCount; col++)    
            {
                for(i = 0; i < A->colCount; i++)
                {
                        sum += 
                            (*(A->pdata + row*(A->colCount) + i)) * 
                            (*(B->pdata + col*(B->colCount) + i));
                }

                *(C->pdata + col + row*(B->rowCount)) *= b;
                *(C->pdata + col + row*(B->rowCount)) += sum*a;

                sum = 0.0;
            }
        }
    }
}

