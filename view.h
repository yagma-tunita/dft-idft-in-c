#ifndef VIEW_H
#define VIEW_H

#include <windows.h>

typedef struct
{
    int           n;
    double        fs;
    const double *time;
    const double *amp;
    const double *reconRe;
    double        maxErr;
    const char   *signalName;
} ViewData;

void ViewDraw(HDC dc, int width, int height, const ViewData *data);

#endif