#include "fft.h"

static const double PI = 3.14159265358979323846;

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

static void BitReverse(ComplexNumber *a, int n)
{
    for (int i = 1, u = 0; i < n; i++) 
    {
        int bit = n >> 1;
        for (; u & bit; bit >>= 1) 
        {
            u ^= bit;
        }
        u ^= bit;
        if (i < u) 
        {
            ComplexNumber temp = a[i];
            a[i] = a[u];
            a[u] = temp;
        }
    }
}

void Fft(const ComplexNumber *input, ComplexNumber *output, int NumSize)
{
    TwiddleInit(NumSize);

    for (int i = 0; i < NumSize; i++) 
    {
        output[i] = input[i];
    }

    BitReverse(output, NumSize);

    for (int len = 2; len <= NumSize; len <<= 1) 
    {
        for (int i = 0; i < NumSize; i += len) 
        {
            for (int u = 0; u < len / 2; u++) 
            {
                int leftIdx  = i + u;
                int rightIdx = i + u + (len / 2);

                int w_index = u * (NumSize / len);
                ComplexNumber W = TwiddleGet(w_index);

                ComplexNumber A = output[leftIdx];
                ComplexNumber B = Mult(output[rightIdx], W);

                output[leftIdx]  = Add(A, B);
                output[rightIdx] = Sub(A, B);
            }
        }
    }
}

void IFft(const ComplexNumber *input, ComplexNumber *output, int NumSize)
{
    TwiddleInit(NumSize);

    for (int i = 0; i < NumSize; i++) 
    {
        output[i] = input[i];
    }

    BitReverse(output, NumSize);

    for (int len = 2; len <= NumSize; len <<= 1) 
    {
        for (int i = 0; i < NumSize; i += len) 
        {
            for (int u = 0; u < len / 2; u++) 
            {
                int leftIdx  = i + u;
                int rightIdx = i + u + (len / 2);

                int w_index = u * (NumSize / len);
                ComplexNumber W = Conjugate(TwiddleGet(w_index));

                ComplexNumber A = output[leftIdx];
                ComplexNumber B = Mult(output[rightIdx], W);

                output[leftIdx]  = Add(A, B);
                output[rightIdx] = Sub(A, B);
            }
        }
    }

    double invN = 1.0 / (double)NumSize;
    for (int i = 0; i < NumSize; i++) 
    {
        output[i] = Scale(output[i], invN);
    }
}
