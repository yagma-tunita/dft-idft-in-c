#ifndef SIGNAL_H
#define SIGNAL_H

void SignalSetSeed(unsigned int seed);
void SignalGenerateSine(double *out, int n, double fs, double freq, double amp, double phase);
void SignalAddSine(double *out, int n, double fs, double freq, double amp, double phase);
void SignalGenerateRandomUniform(double *out, int n, double lo, double hi);
void SignalGenerateRandomGaussian(double *out, int n, double mean, double stddev);

#endif