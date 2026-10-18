#ifndef PLOT_H
#define PLOT_H

#include <windows.h>

typedef void (*PlotDrawFunc)(HDC dc, int width, int height, void *userData);
typedef void (*PlotKeyFunc)(int vkCode, void *userData);

void PlotRun(HINSTANCE hInst, int nCmdShow,
             const char *title, int width, int height,
             PlotDrawFunc draw, PlotKeyFunc key, void *userData);

void PlotDrawCurve(HDC dc, int left, int top, int width, int height,
                   const double *vals, int n,
                   double xMin, double xMax, double yMin, double yMax,
                   const char *title, const char *xLabel, const char *yLabel);

void PlotDrawBars(HDC dc, int left, int top, int width, int height,
                  const double *vals, int n,
                  double xMin, double xMax, double yMax,
                  const char *title, const char *xLabel, const char *yLabel);

void PlotDrawText(HDC dc, int x, int y, const char *text);

#endif