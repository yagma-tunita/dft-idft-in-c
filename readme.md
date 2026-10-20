# fftLib

A small, self-contained Win32 desktop application that demonstrates the **Discrete Fourier Transform (DFT)** and its inverse (**IDFT**) written in plain C. It generates several test signals, computes their spectra, reconstructs them, and renders everything in a native Windows window using only the GDI graphics library.

The project is intentionally minimal: no external dependencies beyond the Windows SDK and the standard C library. All code is C99-compliant and uses English identifiers throughout.

---

## Features

- **Complex number arithmetic** implemented from scratch (`complex.h` / `complex.c`).
- **Forward and inverse transforms** (`fft.h` / `fft.c`):
  - Direct-sum **DFT / IDFT** with `O(N^2)` complexity.
  - Radix-2 **FFT / IFFT** with `O(N log N)` complexity.
- **Precomputed twiddle factors** (`twiddle.h` / `twiddle.c`) for the FFT butterfly stages.
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

A window titled `DFT vs FFT / IDFT vs IFFT` opens and displays four plots arranged in a `2 x 2` grid, each labeled with the elapsed time of the transform that produced it:

1. **DFT Spectrum** (top-left) - magnitude spectrum from `Dft`, scaled as `2|X[k]| / N`.
2. **FFT Spectrum** (top-right) - magnitude spectrum from `Fft`, on the same y-axis scale for direct comparison.
3. **IDFT Reconstruction** (bottom-left) - signal recovered by `IDft`, with the maximum absolute error.
4. **IFFT Reconstruction** (bottom-right) - signal recovered by `IFft`, with the maximum absolute error.

The two spectra and the two reconstructions are drawn with identical axis ranges so that the DFT/FFT and IDFT/IFFT results can be compared visually, while the timing labels show the speed difference between the `O(N^2)` and `O(N log N)` algorithms.

### Controls

| Key | Signal |
|-----|--------|
| `1` | `Sine(5 Hz, 1.0) + Sine(20 Hz, 0.5)` |
| `2` | Uniform random noise in `[-1, 1]` |
| `3` | Gaussian noise `N(0, 0.5^2)` |
| `4` | Unit impulse at sample `0` |

Pressing a key rebuilds the signal, reruns all four transforms, and redraws the window.

---

## Project Structure

```
fftLib/
├── complex.h / complex.c   Complex number type and arithmetic
├── fft.h / fft.c           DFT, IDFT, FFT, and IFFT transforms
├── twiddle.h / twiddle.c   Precomputed FFT twiddle-factor table
├── signal.h / signal.c     Signal generators and PRNG
├── plot.h / plot.c         Win32 GDI plotting primitives
├── view.h / view.c         Composes the three-plot scene
├── main.c                  WinMain entry point and app state
├── Makefile                Build script
└── readme.md               This document
```

### Module responsibilities

- **`complex`** defines `ComplexNumber { double re, im; }` together with helper macros (`Make`, `Add`, `Sub`, `Scale`, `Conjugate`, `MagnitudeSquared`) and functions (`Mult`, `Magnitude`, `Phase`, `ExpI`).
- **`fft`** implements both transform families. `Dft` / `IDft` compute the direct-sum transforms

  ```
  X[k] = sum(u=0..N-1) x[u] * exp(-j 2π k u / N)
  ```

  and the inverse transform scales the conjugate-sum by `1/N`. `Fft` / `IFft` implement the in-place radix-2 Cooley-Tukey algorithm with bit-reversal permutation and a twiddle-factor lookup table.

- **`twiddle`** caches the complex roots of unity `exp(-j 2π i / N)` in a table allocated once per transform size and reused across butterfly stages. `IFft` reuses the same table through conjugation.

- **`signal`** exposes `SignalSetSeed`, `SignalGenerateSine`, `SignalAddSine`, `SignalGenerateRandomUniform`, and `SignalGenerateRandomGaussian`. Random numbers come from a `xorshift64*` generator; the Gaussian sampler uses the Marsaglia polar method and caches the spare sample for efficiency.

- **`plot`** provides `PlotRun` (the message loop and window class), `PlotDrawCurve`, `PlotDrawBars`, and `PlotDrawText`. All drawing is double-buffered through a memory device context to avoid flicker.

- **`view`** lays out the four plots in a `2 x 2` grid, computes shared axis ranges for comparison, and formats the timing labels shown in each title.

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

- Both transform families are called by the demo. `Dft` / `IDft` run in `O(N^2)`; `Fft` / `IFft` run in `O(N log N)` and require a power-of-two `N`. Each plot title shows the measured per-call time (averaged over `REPEAT` runs using the high-resolution `QueryPerformanceCounter`), making the speed difference visible alongside the identical numerical results.
- The PRNG is seeded with a fixed value (`12345`) so results are reproducible across runs.
- All user-facing text, identifiers, and documentation are in English.
