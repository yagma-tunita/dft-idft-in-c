#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>

#include "dft.h"
#include "signal.h"
#include "plot.h"
#include "view.h"

#define N    256
#define FS   256.0

typedef struct
{
    int            n;
    double         fs;
    double        *time;
    ComplexNumber *freq;
    ComplexNumber *recon;
    double         reconRe[N];
    double         amp[N / 2 + 1];
    double         maxErr;
    const char    *signalName;
} AppData;

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

    Dft(app->time, app->freq, app->n);

    for (int k = 0; k <= app->n / 2; k++)
    {
        double a = Magnitude(app->freq[k]);
        if (k == 0) a /= app->n;
        else        a = a * 2.0 / app->n;
        app->amp[k] = a;
    }

    IDft(app->freq, app->recon, app->n);

    app->maxErr = 0.0;
    for (int i = 0; i < app->n; i++)
    {
        app->reconRe[i] = app->recon[i].re;
        double e = fabs(app->reconRe[i] - app->time[i]);
        if (e > app->maxErr) app->maxErr = e;
    }
}

static void DrawScene(HDC dc, int width, int height, void *userData)
{
    AppData *app = (AppData *)userData;

    ViewData vd;
    vd.n          = app->n;
    vd.fs         = app->fs;
    vd.time       = app->time;
    vd.amp        = app->amp;
    vd.reconRe    = app->reconRe;
    vd.maxErr     = app->maxErr;
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

    static AppData app;
    app.n  = N;
    app.fs = FS;

    static double        time[N];
    static ComplexNumber freq[N];
    static ComplexNumber recon[N];

    app.time  = time;
    app.freq  = freq;
    app.recon = recon;

    SignalSetSeed(12345u);
    Rebuild(&app, 1);

    PlotRun(hInst, show,
            "DFT / IDFT Win32 Test  (1..4 to switch signal)",
            960, 760,
            DrawScene, OnKey, &app);

    return 0;
}