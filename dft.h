#ifndef DFT_H
#define DFT_H

#include "complex.h"

void Dft(const double *input, ComplexNumber *output, int NumSize);
void IDft(const ComplexNumber *input, ComplexNumber *output, int NumSize);

#endif