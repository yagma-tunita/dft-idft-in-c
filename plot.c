#include "plot.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define LMARGIN 70
#define TMARGIN 40
#define RMARGIN 22
#define BMARGIN 40

static struct
{
    PlotDrawFunc draw;
    PlotKeyFunc  key;
    void        *userData;
} g_state;

static HFONT CreateUiFont(int height, int weight)
{
    return CreateFontA(height, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                       "Segoe UI");
}

static int DecimalsForStep(double step)
{
    double a = fabs(step);
    int d = 0;
    while (d < 8)
    {
        double r = a - floor(a + 1e-9);
        if (r < 1e-9) break;
        a *= 10.0;
        d++;
    }
    return d;
}

static void NiceTicks(double lo, double hi, int target, double *step, double *first)
{
    double range = hi - lo;
    if (range <= 0.0)
    {
        range = 1.0;
        hi = lo + 1.0;
    }

    double raw = range / (double)target;
    double mag = pow(10.0, floor(log10(raw)));
    double norm = raw / mag;
    double nice;

    if (norm <= 1.5)       nice = 1.0;
    else if (norm <= 2.25) nice = 2.0;
    else if (norm <= 3.75) nice = 2.5;
    else if (norm <= 7.5)  nice = 5.0;
    else                   nice = 10.0;

    *step = nice * mag;
    *first = ceil(lo / *step) * *step;
}

static void DrawPanel(HDC dc, int left, int top, int width, int height)
{
    RECT r;
    r.left = left;
    r.top = top;
    r.right = left + width;
    r.bottom = top + height;

    HBRUSH bg = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(dc, &r, bg);
    DeleteObject(bg);

    HPEN border = CreatePen(PS_SOLID, 1, RGB(214, 218, 225));
    HGDIOBJ oldPen = SelectObject(dc, border);
    HGDIOBJ oldBr = SelectObject(dc, GetStockObject(NULL_BRUSH));
    Rectangle(dc, left, top, left + width, top + height);
    SelectObject(dc, oldBr);
    SelectObject(dc, oldPen);
    DeleteObject(border);
}

static void DrawTitle(HDC dc, int left, int top, const char *title)
{
    if (!title || !*title) return;
    HFONT f = CreateUiFont(-15, FW_SEMIBOLD);
    HGDIOBJ old = SelectObject(dc, f);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(38, 42, 52));
    SetTextAlign(dc, TA_LEFT | TA_TOP);
    TextOutA(dc, left + LMARGIN, top + 6, title, (int)lstrlenA(title));
    SelectObject(dc, old);
    DeleteObject(f);
}

static void DrawGridAndAxes(HDC dc, int gx, int gy, int gw, int gh,
                            double xMin, double xMax, double yMin, double yMax,
                            const char *xLabel, const char *yLabel)
{
    double xStep, xFirst, yStep, yFirst;
    NiceTicks(xMin, xMax, 6, &xStep, &xFirst);
    NiceTicks(yMin, yMax, 4, &yStep, &yFirst);

    int xDec = DecimalsForStep(xStep);
    int yDec = DecimalsForStep(yStep);

    HFONT fTick = CreateUiFont(-13, FW_NORMAL);
    HFONT fLabel = CreateUiFont(-13, FW_SEMIBOLD);
    HGDIOBJ oldFont = SelectObject(dc, fTick);

    HPEN gridPen = CreatePen(PS_DOT, 1, RGB(226, 229, 234));
    HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(148, 154, 164));
    HGDIOBJ oldPen = SelectObject(dc, gridPen);

    char buf[48];

    SetBkMode(dc, TRANSPARENT);

    double spanX = xMax - xMin;
    double spanY = yMax - yMin;
    if (spanX <= 0.0) spanX = 1.0;
    if (spanY <= 0.0) spanY = 1.0;

    for (double v = xFirst; v <= xMax + 1e-9; v += xStep)
    {
        if (v < xMin - 1e-9) continue;
        int x = gx + (int)((v - xMin) / spanX * gw + 0.5);

        MoveToEx(dc, x, gy, NULL);
        LineTo(dc, x, gy + gh);

        SelectObject(dc, axisPen);
        MoveToEx(dc, x, gy + gh, NULL);
        LineTo(dc, x, gy + gh + 5);
        SelectObject(dc, gridPen);

        sprintf(buf, "%.*f", xDec, v);
        SetTextColor(dc, RGB(96, 101, 112));
        SetTextAlign(dc, TA_CENTER | TA_TOP);
        TextOutA(dc, x, gy + gh + 10, buf, (int)strlen(buf));
    }

    for (double v = yFirst; v <= yMax + 1e-9; v += yStep)
    {
        if (v < yMin - 1e-9) continue;
        int y = gy + gh - (int)((v - yMin) / spanY * gh + 0.5);

        MoveToEx(dc, gx, y, NULL);
        LineTo(dc, gx + gw, y);

        SelectObject(dc, axisPen);
        MoveToEx(dc, gx - 5, y, NULL);
        LineTo(dc, gx, y);
        SelectObject(dc, gridPen);

        sprintf(buf, "%.*f", yDec, v);
        SetTextColor(dc, RGB(96, 101, 112));
        SetTextAlign(dc, TA_RIGHT | TA_CENTER);
        TextOutA(dc, gx - 10, y, buf, (int)strlen(buf));
    }

    SelectObject(dc, oldPen);
    DeleteObject(gridPen);
    DeleteObject(axisPen);

    SelectObject(dc, fLabel);
    SetTextColor(dc, RGB(72, 78, 90));

    SetTextAlign(dc, TA_CENTER | TA_TOP);
    if (xLabel && *xLabel)
        TextOutA(dc, gx + gw / 2, gy + gh + 28, xLabel, (int)strlen(xLabel));

    SetTextAlign(dc, TA_LEFT | TA_TOP);
    if (yLabel && *yLabel)
        TextOutA(dc, gx - LMARGIN + 2, gy - TMARGIN + 6, yLabel, (int)strlen(yLabel));

    SelectObject(dc, oldFont);
    DeleteObject(fTick);
    DeleteObject(fLabel);
}

void PlotDrawCurve(HDC dc, int left, int top, int width, int height,
                   const double *vals, int n,
                   double xMin, double xMax, double yMin, double yMax,
                   const char *title, const char *xLabel, const char *yLabel)
{
    DrawPanel(dc, left, top, width, height);
    DrawTitle(dc, left, top, title);

    int gx = left + LMARGIN;
    int gy = top + TMARGIN;
    int gw = width - LMARGIN - RMARGIN;
    int gh = height - TMARGIN - BMARGIN;

    DrawGridAndAxes(dc, gx, gy, gw, gh, xMin, xMax, yMin, yMax, xLabel, yLabel);

    if (n < 2 || gw < 4 || gh < 4) return;

    double spanX = (xMax > xMin) ? (xMax - xMin) : 1.0;
    double spanY = (yMax > yMin) ? (yMax - yMin) : 1.0;

    POINT *pts = (POINT *)malloc(sizeof(POINT) * (size_t)n);
    if (!pts) return;

    for (int i = 0; i < n; i++)
    {
        double xVal = xMin + spanX * i / (double)(n - 1);
        double yVal = vals[i];
        if (yVal < yMin) yVal = yMin;
        if (yVal > yMax) yVal = yMax;

        pts[i].x = gx + (int)((xVal - xMin) / spanX * gw + 0.5);
        pts[i].y = gy + gh - (int)((yVal - yMin) / spanY * gh + 0.5);
    }

    POINT *poly = (POINT *)malloc(sizeof(POINT) * (size_t)(n + 2));
    if (poly)
    {
        poly[0].x = pts[0].x;
        poly[0].y = gy + gh;
        for (int i = 0; i < n; i++) poly[i + 1] = pts[i];
        poly[n + 1].x = pts[n - 1].x;
        poly[n + 1].y = gy + gh;

        HBRUSH fillBrush = CreateSolidBrush(RGB(206, 226, 250));
        HPEN fillPen = CreatePen(PS_SOLID, 1, RGB(206, 226, 250));
        HGDIOBJ oldBr = SelectObject(dc, fillBrush);
        HGDIOBJ oldPen = SelectObject(dc, fillPen);
        Polygon(dc, poly, n + 2);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBr);
        DeleteObject(fillPen);
        DeleteObject(fillBrush);
        free(poly);
    }

    HPEN curvePen = CreatePen(PS_SOLID, 2, RGB(30, 110, 220));
    HGDIOBJ oldPen = SelectObject(dc, curvePen);
    Polyline(dc, pts, n);
    SelectObject(dc, oldPen);
    DeleteObject(curvePen);

    if (n <= 100)
    {
        HBRUSH mBr = CreateSolidBrush(RGB(255, 255, 255));
        HPEN mPen = CreatePen(PS_SOLID, 1, RGB(30, 110, 220));
        HGDIOBJ oldBr = SelectObject(dc, mBr);
        oldPen = SelectObject(dc, mPen);
        for (int i = 0; i < n; i++)
            Ellipse(dc, pts[i].x - 3, pts[i].y - 3, pts[i].x + 3, pts[i].y + 3);
        SelectObject(dc, oldPen);
        SelectObject(dc, oldBr);
        DeleteObject(mPen);
        DeleteObject(mBr);
    }

    free(pts);
}

void PlotDrawBars(HDC dc, int left, int top, int width, int height,
                  const double *vals, int n,
                  double xMin, double xMax, double yMax,
                  const char *title, const char *xLabel, const char *yLabel)
{
    DrawPanel(dc, left, top, width, height);
    DrawTitle(dc, left, top, title);

    int gx = left + LMARGIN;
    int gy = top + TMARGIN;
    int gw = width - LMARGIN - RMARGIN;
    int gh = height - TMARGIN - BMARGIN;

    if (yMax <= 0.0)
    {
        yMax = 0.0;
        for (int i = 0; i < n; i++)
            if (vals[i] > yMax) yMax = vals[i];
        if (yMax <= 0.0) yMax = 1.0;
    }

    DrawGridAndAxes(dc, gx, gy, gw, gh, xMin, xMax, 0.0, yMax, xLabel, yLabel);

    if (n <= 0 || gw < 4 || gh < 4) return;

    double spanX = (xMax > xMin) ? (xMax - xMin) : 1.0;
    double denom = (n > 1) ? (double)(n - 1) : 1.0;

    int slotW = gw / n;
    int barW = slotW;
    if (barW < 2) barW = 2;
    if (barW > 64) barW = 64;

    HBRUSH brTop = CreateSolidBrush(RGB(86, 150, 255));
    HBRUSH brBot = CreateSolidBrush(RGB(36, 98, 212));
    HPEN edge = CreatePen(PS_SOLID, 1, RGB(22, 72, 158));

    for (int i = 0; i < n; i++)
    {
        double xVal = xMin + spanX * i / denom;
        int cx = gx + (int)((xVal - xMin) / spanX * gw + 0.5);

        double yVal = vals[i];
        if (yVal < 0.0) yVal = 0.0;
        if (yVal > yMax) yVal = yMax;

        int bh = (int)(yVal / yMax * gh + 0.5);
        if (bh < 1) bh = 1;

        int bx0 = cx - barW / 2;
        int bx1 = cx + barW / 2;
        int by0 = gy + gh - bh;
        int by1 = gy + gh;
        int mid = by0 + (int)((by1 - by0) * 0.45);

        SelectObject(dc, brBot);
        SelectObject(dc, GetStockObject(NULL_PEN));
        Rectangle(dc, bx0, by0, bx1, by1);

        SelectObject(dc, brTop);
        Rectangle(dc, bx0, by0, bx1, mid);

        SelectObject(dc, GetStockObject(NULL_BRUSH));
        SelectObject(dc, edge);
        Rectangle(dc, bx0, by0, bx1, by1);
    }

    DeleteObject(brTop);
    DeleteObject(brBot);
    DeleteObject(edge);
}

void PlotDrawText(HDC dc, int x, int y, const char *text)
{
    if (!text) return;
    HFONT f = CreateUiFont(-13, FW_NORMAL);
    HGDIOBJ old = SelectObject(dc, f);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(110, 116, 126));
    SetTextAlign(dc, TA_LEFT | TA_TOP);
    TextOutA(dc, x, y, text, (int)lstrlenA(text));
    SelectObject(dc, old);
    DeleteObject(f);
}

static LRESULT CALLBACK PlotWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        FillRect(memDC, &rc, (HBRUSH)GetSysColorBrush(COLOR_BTNFACE));
        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(50, 50, 50));

        if (g_state.draw)
            g_state.draw(memDC, rc.right, rc.bottom, g_state.userData);

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_KEYDOWN:
        if (g_state.key) g_state.key((int)wParam, g_state.userData);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void PlotRun(HINSTANCE hInst, int nCmdShow, const char *title, int width, int height,
             PlotDrawFunc draw, PlotKeyFunc key, void *userData)
{
    g_state.draw = draw;
    g_state.key = key;
    g_state.userData = userData;

    const char *cls = "PlotWindowClass";

    WNDCLASSA wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = PlotWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
    wc.lpszClassName = cls;
    RegisterClassA(&wc);

    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = width;
    rc.bottom = height;
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowA(cls, title, WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              rc.right - rc.left, rc.bottom - rc.top,
                              NULL, NULL, hInst, NULL);
    if (!hwnd) return;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
