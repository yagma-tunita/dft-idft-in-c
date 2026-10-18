#include "dft.h"

const double PI = 3.14159265358979323846;

void Dft(const double *input, ComplexNumber *output, int NumSize)
{
    for(int k = 0; k < NumSize; k++)
    {
        ComplexNumber Xk = Make(0.0, 0.0);
        for(int u = 0; u < NumSize; u++)
        {
            double theta = -2.0 * PI * (double)k * (double)u / (double)NumSize;
            ComplexNumber basis = ExpI(theta);
            ComplexNumber term = Scale(basis, input[u]);
            Xk = Add(Xk, term);
        }
        output[k] = Xk;
    }
}

void IDft(const ComplexNumber *input, ComplexNumber *output, int NumSize)
{
    double invN = 1.0 / NumSize;
    for(int u = 0; u < NumSize; u++)
    {
        ComplexNumber Xu = Make(0.0, 0.0);
        for(int k = 0; k < NumSize; k++)
        {
            double theta = 2.0 * PI * (double)k * (double)u / (double)NumSize;
            ComplexNumber basis = ExpI(theta);
            ComplexNumber term = Mult(input[k], basis); 
            Xu = Add(Xu, term);
        }
        output[u] = Scale(Xu, invN);
    }
}
