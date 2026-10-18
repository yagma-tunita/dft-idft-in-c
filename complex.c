#include "complex.h"
#include <math.h>

ComplexNumber Mult(ComplexNumber a, ComplexNumber b)
{
    ComplexNumber out;
    out.re = a.re * b.re - a.im * b.im;
    out.im = a.re * b.im + a.im * b.re;
    return out;
}

double Magnitude(ComplexNumber a)
{
    return sqrt(MagnitudeSquared(a));
}

double Phase(ComplexNumber a)
{
    return atan2(a.im,a.re);
}

ComplexNumber ExpI(double theta)
{
    double re = cos(theta);
    double im = sin(theta);
    return Make(re,im);
}
