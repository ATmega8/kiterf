#include "matrix.h"

/* A = A+B*/
void MATRIX_Add(MATRIX_TypeDef* A, const MATRIX_TypeDef* B)
{
    uint32_t row, col;    

    /*检查维度是否匹配*/
    MATRIX_ASSERT((A->rowCount == B->rowCount) &
                  (A->colCount == B->colCount) ); 

    /*相加*/
    for(row = 0; row < A->rowCount; row++)
    {
        for(col = 0; col < A->colCount; col++)    
        {
            *(A->pdata + col + row*(A->colCount)) = 
                *(A->pdata + col + row*(A->colCount)) + *(B->pdata + col + row*(A->colCount));
        }
    }
}

