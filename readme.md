# fftLib

A small, self-contained Win32 desktop application that demonstrates the **Discrete Fourier Transform (DFT)** and its inverse (**IDFT**) written in plain C. It generates several test signals, computes their spectra, reconstructs them, and renders everything in a native Windows window using only the GDI graphics library.

The project is intentionally minimal: no external dependencies beyond the Windows SDK and the standard C library. All code is C99-compliant and uses English identifiers throughout.

---

## Features

- **Complex number arithmetic** implemented from scratch (`complex.h` / `complex.c`).
- **Forward and inverse DFT** with `O(N^2)` complexity (`dft.h` / `dft.c`).
- **Signal generators** (`signal.h` / `signal.c`):
  - Pure and additive sine waves.
  - Uniform random noise.
  - Gaussian (normal) random noise via the Marsaglia polar method.
  - Deterministic seeding via a high-quality `xorshift64*` PRNG.
- **Native Win32 plotting** (`plot.h` / `plot.c`) with:
  - Anti-aliased `Segoe UI` fonts.
  - Automatic "nice" axis tick steps and adaptive decimal formatting.
  - Dotted grid lines, axis labels, titles, and styled data markers.
  - Filled area curves and two-tone bar charts.
- **Interactive signal switching** with the keyboard (`1`–`4`).
- **Reconstruction error reporting** between the IDFT output and the original signal.

---

## Requirements

| Component | Requirement |
|-----------|-------------|
| OS        | Windows (7 or later) |
| Compiler  | GCC (MinGW-w64) |
| Build tool| GNU Make |
| Libraries | `gdi32`, `user32`, `m` (all shipped with MinGW-w64) |

The code uses only the Win32 API, so no Visual Studio or third-party toolkits are required.

---

## Building

Open a terminal in the project directory and run:

```sh
mingw32-make
```

This produces `run.exe`. To clean all build artifacts:

```sh
mingw32-make clean
```

The `Makefile` compiles with `-O2 -Wall -Wextra` and links with `-mwindows` so that no console window is shown at launch.

---

## Running

```sh
run.exe
```

A window titled `DFT / IDFT Win32 Test` opens and displays three stacked plots:

1. **Time Domain** - the current input signal plotted over time in seconds.
2. **Amplitude Spectrum** - the magnitude spectrum for bins `k = 0 .. N/2`, scaled as `2|X[k]| / N`.
3. **IDFT Reconstruction** - the signal recovered from the full spectrum, with the maximum absolute reconstruction error shown in the title.

### Controls

| Key | Signal |
|-----|--------|
| `1` | `Sine(5 Hz, 1.0) + Sine(20 Hz, 0.5)` |
| `2` | Uniform random noise in `[-1, 1]` |
| `3` | Gaussian noise `N(0, 0.5^2)` |
| `4` | Unit impulse at sample `0` |

Pressing a key rebuilds the signal, recomputes the DFT/IDFT, and redraws the window.

---

## Project Structure

```
fftLib/
├── complex.h / complex.c   Complex number type and arithmetic
├── dft.h / dft.c           Forward and inverse DFT
├── signal.h / signal.c     Signal generators and PRNG
├── plot.h / plot.c         Win32 GDI plotting primitives
├── view.h / view.c         Composes the three-plot scene
├── main.c                  WinMain entry point and app state
├── Makefile                Build script
└── readme.md               This document
```

### Module responsibilities

- **`complex`** defines `ComplexNumber { double re, im; }` together with helper macros (`Make`, `Add`, `Sub`, `Scale`, `Conjugate`, `MagnitudeSquared`) and functions (`Mult`, `Magnitude`, `Phase`, `ExpI`).
- **`dft`** implements the direct-sum DFT and IDFT. The forward transform computes

  ```
  X[k] = sum(u=0..N-1) x[u] * exp(-j 2π k u / N)
  ```

  and the inverse transform scales the conjugate-sum by `1/N`.

- **`signal`** exposes `SignalSetSeed`, `SignalGenerateSine`, `SignalAddSine`, `SignalGenerateRandomUniform`, and `SignalGenerateRandomGaussian`. Random numbers come from a `xorshift64*` generator; the Gaussian sampler uses the Marsaglia polar method and caches the spare sample for efficiency.

- **`plot`** provides `PlotRun` (the message loop and window class), `PlotDrawCurve`, `PlotDrawBars`, and `PlotDrawText`. All drawing is double-buffered through a memory device context to avoid flicker.

- **`view`** lays out the three subplots and computes sensible axis ranges and units.

- **`main`** holds the application state, handles the `1`–`4` key bindings, and seeds the PRNG with a fixed value for reproducibility.

---

## Configuration

The demo constants are defined at the top of `main.c`:

```c
#define N    256
#define FS   256.0
```

- `N` is the number of samples (also the DFT size).
- `FS` is the sample rate in Hz.

With `FS == N`, each DFT bin `k` corresponds directly to `k` Hz, so the spectrum x-axis spans `0` to `128` Hz.

---

## Notes

- The DFT here is the textbook `O(N^2)` implementation. It is intended for teaching and verification, not for performance-critical work. For large inputs, an FFT would be required.
- The PRNG is seeded with a fixed value (`12345`) so results are reproducible across runs.
- All user-facing text, identifiers, and documentation are in English.
