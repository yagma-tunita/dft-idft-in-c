#include "view.h"
#include "plot.h"
#include <stdio.h>
#include <math.h>

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

static double MaxVal(const double *v, int n)
{
    double m = 0.0;
    for (int i = 0; i < n; i++)
        if (v[i] > m) m = v[i];
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

void ViewDraw(HDC dc, int width, int height, const ViewData *data)
{
    const int margin = 24;
    const int gap = 20;
    const int footerH = 26;

    int w = width - 2 * margin;
    int h = (height - 2 * margin - 2 * gap - footerH) / 3;

    int y1 = margin;
    int y2 = y1 + h + gap;
    int y3 = y2 + h + gap;

    double xMax = (data->n > 1) ? (double)(data->n - 1) / data->fs : 0.0;

    double yAmp = NiceCeiling(MaxAbs(data->time, data->n) * 1.15);

    char buf[220];
    wsprintfA(buf, "Time Domain - %s", data->signalName);
    PlotDrawCurve(dc, margin, y1, w, h, data->time, data->n,
                  0.0, xMax, -yAmp, yAmp, buf, "Time (s)", "Amplitude");

    double specMax = MaxVal(data->amp, data->n / 2 + 1) * 1.1;
    if (specMax < 1e-12) specMax = 1.0;
    PlotDrawBars(dc, margin, y2, w, h, data->amp, data->n / 2 + 1,
                 0.0, data->fs / 2.0, specMax,
                 "Amplitude Spectrum", "Frequency (Hz)", "|X[k]|");

    wsprintfA(buf, "IDFT Reconstruction (max |error| = %.3e)", data->maxErr);
    PlotDrawCurve(dc, margin, y3, w, h, data->reconRe, data->n,
                  0.0, xMax, -yAmp, yAmp, buf, "Time (s)", "Amplitude");

    PlotDrawText(dc, margin, height - footerH,
                 "Keys: 1 = Sine   2 = Uniform   3 = Gaussian   4 = Impulse");
}
