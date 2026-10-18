#include "signal.h"
#include <math.h>

static const double PI = 3.14159265358979323846;

static unsigned long long g_state = 0x9E3779B97F4A7C15ULL;
static int g_hasSpare = 0;
static double g_spare = 0.0;

static unsigned long long NextU64(void)
{
    unsigned long long x = g_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    g_state = x;
    return x * 0x2545F4914F6CDD1DULL;
}

void SignalSetSeed(unsigned int seed)
{
    g_state = (seed != 0u) ? (unsigned long long)seed : 0x9E3779B97F4A7C15ULL;
    for (int i = 0; i < 16; i++) NextU64();
}

static double Rand01(void)
{
    return (double)(NextU64() >> 11) * (1.0 / 9007199254740992.0);
}

static double RandGaussian(void)
{
    if (g_hasSpare)
    {
        g_hasSpare = 0;
        return g_spare;
    }

    double x, y, s;
    do
    {
        x = 2.0 * Rand01() - 1.0;
        y = 2.0 * Rand01() - 1.0;
        s = x * x + y * y;
    } while (s >= 1.0 || s <= 0.0);

    double factor = sqrt(-2.0 * log(s) / s);
    g_spare = y * factor;
    g_hasSpare = 1;
    return x * factor;
}

void SignalGenerateSine(double *out, int n, double fs, double freq, double amp, double phase)
{
    for (int i = 0; i < n; i++)
    {
        double t = (double)i / fs;
        out[i] = amp * sin(2.0 * PI * freq * t + phase);
    }
}

void SignalAddSine(double *out, int n, double fs, double freq, double amp, double phase)
{
    for (int i = 0; i < n; i++)
    {
        double t = (double)i / fs;
        out[i] += amp * sin(2.0 * PI * freq * t + phase);
    }
}

void SignalGenerateRandomUniform(double *out, int n, double lo, double hi)
{
    for (int i = 0; i < n; i++)
    {
        out[i] = lo + (hi - lo) * Rand01();
    }
}

void SignalGenerateRandomGaussian(double *out, int n, double mean, double stddev)
{
    for (int i = 0; i < n; i++)
    {
        out[i] = mean + stddev * RandGaussian();
    }
}
