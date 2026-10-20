#ifndef FFT_H
#define FFT_H

#include "twiddle.h"

void Dft(const double *input, ComplexNumber *output, int NumSize);
void IDft(const ComplexNumber *input, ComplexNumber *output, int NumSize);
void Fft(const ComplexNumber *input, ComplexNumber *output, int NumSize);
void IFft(const ComplexNumber *input, ComplexNumber *output, int NumSize);

#endif