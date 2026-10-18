#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct
{
    double re;
    double im;
}ComplexNumber;

#define Make(re,im) ((ComplexNumber){(re), (im)})

#define Add(a,b) ((ComplexNumber){(a).re + (b).re, (a).im + (b).im})
#define Sub(a,b) ((ComplexNumber){(a).re - (b).re, (a).im - (b).im})
ComplexNumber Mult(ComplexNumber a, ComplexNumber b);
#define Scale(a,s) ((ComplexNumber){(a).re * (s), (a).im * (s)})

#define Conjugate(a) ((ComplexNumber){(a).re, -(a).im})
#define MagnitudeSquared(a) (((a).re * (a).re) + ((a).im * (a).im))
double Magnitude(ComplexNumber a);

double Phase(ComplexNumber a);
ComplexNumber ExpI(double theta);


#endif