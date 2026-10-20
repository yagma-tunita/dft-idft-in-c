#include "view.h"
#include "plot.h"
#include <stdio.h>
#include <math.h>

static double MaxVal(const double *v, int n)
{
    double m = 0.0;
    for (int i = 0; i < n; i++)
        if (v[i] > m) m = v[i];
    return m;
}

static double MaxAbs(const double *v, int n)
{
    double m = 0.0;
    for (int i = 0; i < n; i++)
    {
        double a = fabs(v[i]);
        if (a > m) m = a;
    }
    return m;
}

static double NiceCeiling(double v)
{
    if (v <= 0.0) return 1.0;
    double mag = pow(10.0, floor(log10(v)));
    double norm = v / mag;
    double nice;
    if (norm <= 1.0) nice = 1.0;
    else if (norm <= 2.0) nice = 2.0;
    else if (norm <= 5.0) nice = 5.0;
    else nice = 10.0;
    return nice * mag;
}

static void FormatTime(double ms, char *buf)
{
    if (ms < 0.01) sprintf(buf, "%.2f us", ms * 1000.0);
    else           sprintf(buf, "%.3f ms", ms);
}

void ViewDraw(HDC dc, int width, int height, const ViewData *data)
{
    const int margin = 24;
    const int gap = 20;
    const int footerH = 26;

    int plotW = (width - 2 * margin - gap) / 2;
    int plotH = (height - 2 * margin - gap - footerH) / 2;

    int x1 = margin;
    int x2 = margin + plotW + gap;
    int y1 = margin;
    int y2 = margin + plotH + gap;

    double xMaxFreq = data->fs / 2.0;
    double xMaxTime = (data->n > 1) ? (double)(data->n - 1) / data->fs : 0.0;

    int half = data->n / 2 + 1;

    double specMax = MaxVal(data->ampDft, half);
    double f = MaxVal(data->ampFft, half);
    if (f > specMax) specMax = f;
    specMax *= 1.1;
    if (specMax < 1e-12) specMax = 1.0;

    double recAmp = MaxAbs(data->reconDftRe, data->n);
    f = MaxAbs(data->reconFftRe, data->n);
    if (f > recAmp) recAmp = f;
    recAmp = NiceCeiling(recAmp * 1.15);

    char timeBuf[32];
    char buf[256];

    FormatTime(data->tDft, timeBuf);
    wsprintfA(buf, "DFT Spectrum  (%s)", timeBuf);
    PlotDrawBars(dc, x1, y1, plotW, plotH, data->ampDft, half,
                 0.0, xMaxFreq, specMax, buf, "Frequency (Hz)", "|X[k]|");

    FormatTime(data->tFft, timeBuf);
    wsprintfA(buf, "FFT Spectrum  (%s)", timeBuf);
    PlotDrawBars(dc, x2, y1, plotW, plotH, data->ampFft, half,
                 0.0, xMaxFreq, specMax, buf, "Frequency (Hz)", "|X[k]|");

    FormatTime(data->tIDft, timeBuf);
    wsprintfA(buf, "IDFT Reconstruction  (%s, max err %.2e)", timeBuf, data->errDft);
    PlotDrawCurve(dc, x1, y2, plotW, plotH, data->reconDftRe, data->n,
                  0.0, xMaxTime, -recAmp, recAmp, buf, "Time (s)", "Amplitude");

    FormatTime(data->tIFft, timeBuf);
    wsprintfA(buf, "IFFT Reconstruction  (%s, max err %.2e)", timeBuf, data->errFft);
    PlotDrawCurve(dc, x2, y2, plotW, plotH, data->reconFftRe, data->n,
                  0.0, xMaxTime, -recAmp, recAmp, buf, "Time (s)", "Amplitude");

    wsprintfA(buf, "%s    Keys: 1 = Sine   2 = Uniform   3 = Gaussian   4 = Impulse",
              data->signalName);
    PlotDrawText(dc, margin, height - footerH, buf);
}
