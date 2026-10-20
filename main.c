#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>

#include "fft.h"
#include "signal.h"
#include "plot.h"
#include "view.h"

#define N      256
#define FS     256.0
#define REPEAT 20

typedef struct
{
    int            n;
    double         fs;
    double        *time;
    ComplexNumber *timeComplex;
    ComplexNumber *freqDft;
    ComplexNumber *freqFft;
    ComplexNumber *reconDft;
    ComplexNumber *reconFft;
    double         ampDft[N / 2 + 1];
    double         ampFft[N / 2 + 1];
    double         reconDftRe[N];
    double         reconFftRe[N];
    double         errDft;
    double         errFft;
    double         tDft;
    double         tFft;
    double         tIDft;
    double         tIFft;
    const char    *signalName;
} AppData;

static LARGE_INTEGER g_freq;

static double NowMs(void)
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)g_freq.QuadPart;
}

static double TimeDft(const double *in, ComplexNumber *out, int n)
{
    double t0 = NowMs();
    for (int r = 0; r < REPEAT; r++) Dft(in, out, n);
    return (NowMs() - t0) / (double)REPEAT;
}

static double TimeFft(const ComplexNumber *in, ComplexNumber *out, int n)
{
    double t0 = NowMs();
    for (int r = 0; r < REPEAT; r++) Fft(in, out, n);
    return (NowMs() - t0) / (double)REPEAT;
}

static double TimeIDft(const ComplexNumber *in, ComplexNumber *out, int n)
{
    double t0 = NowMs();
    for (int r = 0; r < REPEAT; r++) IDft(in, out, n);
    return (NowMs() - t0) / (double)REPEAT;
}

static double TimeIFft(const ComplexNumber *in, ComplexNumber *out, int n)
{
    double t0 = NowMs();
    for (int r = 0; r < REPEAT; r++) IFft(in, out, n);
    return (NowMs() - t0) / (double)REPEAT;
}

static void ComputeAmp(const ComplexNumber *freq, double *amp, int n)
{
    for (int k = 0; k <= n / 2; k++)
    {
        double a = Magnitude(freq[k]);
        if (k == 0) a /= (double)n;
        else        a = a * 2.0 / (double)n;
        amp[k] = a;
    }
}

static double ComputeError(const double *a, const double *b, int n)
{
    double m = 0.0;
    for (int i = 0; i < n; i++)
    {
        double e = fabs(a[i] - b[i]);
        if (e > m) m = e;
    }
    return m;
}

static void Rebuild(AppData *app, int type)
{
    switch (type)
    {
    case 1:
        SignalGenerateSine(app->time, app->n, app->fs, 5.0, 1.0, 0.0);
        SignalAddSine(app->time, app->n, app->fs, 20.0, 0.5, 0.0);
        app->signalName = "Sine(5Hz, 1.0) + Sine(20Hz, 0.5)";
        break;

    case 2:
        SignalGenerateRandomUniform(app->time, app->n, -1.0, 1.0);
        app->signalName = "Uniform random noise [-1, 1]";
        break;

    case 3:
        SignalGenerateRandomGaussian(app->time, app->n, 0.0, 0.5);
        app->signalName = "Gaussian noise N(0, 0.5^2)";
        break;

    case 4:
        for (int i = 0; i < app->n; i++)
            app->time[i] = (i == 0) ? 1.0 : 0.0;
        app->signalName = "Impulse at u=0";
        break;
    }

    for (int i = 0; i < app->n; i++)
    {
        app->timeComplex[i].re = app->time[i];
        app->timeComplex[i].im = 0.0;
    }

    app->tDft  = TimeDft(app->time, app->freqDft, app->n);
    app->tFft  = TimeFft(app->timeComplex, app->freqFft, app->n);
    app->tIDft = TimeIDft(app->freqDft, app->reconDft, app->n);
    app->tIFft = TimeIFft(app->freqFft, app->reconFft, app->n);

    ComputeAmp(app->freqDft, app->ampDft, app->n);
    ComputeAmp(app->freqFft, app->ampFft, app->n);

    for (int i = 0; i < app->n; i++)
    {
        app->reconDftRe[i] = app->reconDft[i].re;
        app->reconFftRe[i] = app->reconFft[i].re;
    }

    app->errDft = ComputeError(app->reconDftRe, app->time, app->n);
    app->errFft = ComputeError(app->reconFftRe, app->time, app->n);
}

static void DrawScene(HDC dc, int width, int height, void *userData)
{
    AppData *app = (AppData *)userData;

    ViewData vd;
    vd.n          = app->n;
    vd.fs         = app->fs;
    vd.ampDft     = app->ampDft;
    vd.ampFft     = app->ampFft;
    vd.reconDftRe = app->reconDftRe;
    vd.reconFftRe = app->reconFftRe;
    vd.errDft     = app->errDft;
    vd.errFft     = app->errFft;
    vd.tDft       = app->tDft;
    vd.tFft       = app->tFft;
    vd.tIDft      = app->tIDft;
    vd.tIFft      = app->tIFft;
    vd.signalName = app->signalName;

    ViewDraw(dc, width, height, &vd);
}

static void OnKey(int vk, void *userData)
{
    AppData *app = (AppData *)userData;
    if (vk >= '1' && vk <= '4')
        Rebuild(app, vk - '0');
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show)
{
    (void)hPrev;
    (void)cmd;

    QueryPerformanceFrequency(&g_freq);

    static AppData app;
    app.n  = N;
    app.fs = FS;

    static double        time[N];
    static ComplexNumber timeComplex[N];
    static ComplexNumber freqDft[N];
    static ComplexNumber freqFft[N];
    static ComplexNumber reconDft[N];
    static ComplexNumber reconFft[N];

    app.time        = time;
    app.timeComplex = timeComplex;
    app.freqDft     = freqDft;
    app.freqFft     = freqFft;
    app.reconDft    = reconDft;
    app.reconFft    = reconFft;

    SignalSetSeed(12345u);
    TwiddleInit(N);
    Rebuild(&app, 1);

    PlotRun(hInst, show,
            "DFT vs FFT / IDFT vs IFFT  (1..4 to switch signal)",
            960, 760,
            DrawScene, OnKey, &app);

    TwiddleFree();
    return 0;
}
