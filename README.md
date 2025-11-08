## Mandelbrot - CS361 Homework 5

A C++ program that renders the Mandelbrot set to 24-bit BMP files.
The implementation is multithreaded by a single "manager" thread enqueues row work items and multiple worker threads render rows in parallel into an in-memory BGR buffer which is then written to disk as a BMP.

## Author

- By: Samii Shabuse
- Class: CS361: Concurrent Programming

## Project layout

- `Makefile` — build targets and example BMP targets (`e0.bmp` .. `e4.bmp`).
- `bin/mandelbrot` — build output (created by the Makefile).
- `src/mandelbrot.cpp` — main program: argument parsing, tiling/dispatching, iteration, coloring, multithreading.
- `src/bitmap.h` / `src/bitmap.cpp` — BMP writer (public function `write_bmp_24`).
- `src/multithread.cpp` — small `JobQueue` implementation used by the manager/worker threads.

## Requirements

- A C++17-capable compiler (g++ recommended).
- POSIX threads support (the Makefile uses `-pthread`).

## Build

From the repository root run:

```bash
make
```

This will create `bin/mandelbrot`.

## Usage

The program prints a short usage message when called with the wrong number of arguments. The expected arguments are:

```
mandelbrot x_min x_max y_min y_max out_file
```

- `x_min`, `x_max`, `y_min`, `y_max` — rectangle in the complex plane to render (long double precision parsed with `stold`).
- `out_file` — path to write a 24-bit BMP image.

Example (also provided as Makefile targets):

```bash
./bin/mandelbrot -2 1 -1.5 1.5 e0.bmp
./bin/mandelbrot -0.5 0 0.3 1.2 e1.bmp
```

Or use the Makefile targets:

```bash
make e0.bmp
make e1.bmp
```

## Implementation details

- Image width is defined by `WIDTH` in `src/mandelbrot.cpp` (default 1500). The program computes the height to preserve aspect ratio:

	HEIGHT = round(WIDTH * (y_range / x_range)).

- Iterations: `MAX_ITERATIONS` (default 100 because of homework instructions) is used as the escape threshold. The iteration function returns the number of iterations before |z|^2 > 4 or `MAX_ITERATIONS` if the point appears to be in the set.

- Coloring: a function `mandelbrot_colorize(int it)` maps an iteration count to a simple RGB palette. Points inside the set (no escape) are colored black.

- Multithreading: `JobQueue` holds integer row indices. A manager thread enqueues every row index and calls `close()`; worker threads pop indices and render each row into the shared `bgr` buffer. Each worker writes to distinct row ranges, so no synchronization is required for pixel writes.

- BMP writer: `write_bmp_24` writes a 24-bit BMP file with rows padded to a multiple of 4 bytes and writes rows bottom-to-top as the BMP format requires.

