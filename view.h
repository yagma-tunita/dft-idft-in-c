#ifndef VIEW_H
#define VIEW_H

#include <windows.h>

typedef struct
{
    int           n;
    double        fs;
    const double *ampDft;
    const double *ampFft;
    const double *reconDftRe;
    const double *reconFftRe;
    double        errDft;
    double        errFft;
    double        tDft;
    double        tFft;
    double        tIDft;
    double        tIFft;
    const char   *signalName;
} ViewData;

void ViewDraw(HDC dc, int width, int height, const ViewData *data);

#endif
