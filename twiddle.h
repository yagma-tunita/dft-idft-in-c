#ifndef TWIDDLE_H
#define TWIDDLE_H

#include "complex.h"

void TwiddleInit(int NumSize);
ComplexNumber TwiddleGet(int index);
void TwiddleFree(void);

#endif
