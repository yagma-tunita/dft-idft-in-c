#include "twiddle.h"
#include <stdlib.h>

static const double PI = 3.14159265358979323846;

static ComplexNumber *TableW = NULL;
static int TableSize = 0;

void TwiddleInit(int NumSize)
{
    if(TableW != NULL && TableSize == NumSize)return;

    if(TableW != NULL)
    {
        TwiddleFree();
        TableW = NULL;
    }

    TableSize = NumSize;
    TableW = (ComplexNumber*)malloc(sizeof(ComplexNumber) * TableSize);
    if(TableW == NULL)
    {
        TableSize = 0;
        return;
    }

    for (int i = 0; i < TableSize; i++) 
    {
        double theta = -2.0 * PI * i / TableSize;
        TableW[i] = ExpI(theta);
    }
}

ComplexNumber TwiddleGet(int index)
{
    if(TableW == NULL || TableSize == 0) return Make(0.0, 0.0);

    index %= TableSize;
    if(index < 0)index += TableSize;

    return TableW[index];
}

void TwiddleFree(void)
{
    if (TableW != NULL) 
    {
        free(TableW);
        TableW = NULL;
    }
    TableSize = 0;
}
