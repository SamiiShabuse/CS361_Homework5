/**
 * @file src/mandelbrot.cpp
 * 
 * @brief Generates a Mandelbrot set image and saves it as a BMP file.
 * 
 * @author Samii Shabuse <sus24@drexel.edu>
 * @date November 7, 2025
 * 
 * @section Overview
 * 
 * This file contains the main function to generate a Mandelbrot set image. It uses
 * multithreading to speed up the rendering process and saves the output as a BMP file.
 */

#include <bits/stdc++.h>
#include "multithread.cpp"
#include "bitmap.h"

// Maximum iterations for Mandelbrot calculation based on homework instructions
static const int MAX_ITERATIONS = 100; 

// Rectangle representing the area of the complex plane to render
struct Rectangle { long double x_min, x_max, y_min, y_max; };

// Image width
static const int WIDTH = 1500;

// Image height
int HEIGHT; // This will be determined by the number of points on the y axis.

/**
 * @brief Computes the number of iterations for a point in the Mandelbrot set.
 * 
 * @param real The real part of the complex number.
 * @param imag The imaginary part of the complex number.
 * 
 * @return int The number of iterations before escape (up to MAX_ITERATIONS).
 * 
 * @details The function iteratively applies the Mandelbrot formula:
 *          z = z^2 + c, where c is the complex number represented by (real, imag).
 *          The iteration continues until the magnitude of z exceeds 2 or the maximum
 *          number of iterations is reached.
 */
int mandelbrot_iterations(long double real, long double imag) {
    long double z_real = 0.0;
    long double z_imag = 0.0;
    int iterations = 0;

    while(z_real * z_real + z_imag * z_imag <= 4.0 && iterations < MAX_ITERATIONS) {
        long double temp_real = z_real * z_real - z_imag * z_imag + real;
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = temp_real;
        iterations++;
    }
    return iterations;
}

/**
 * @brief Maps pixel coordinates to complex plane coordinates.
 * 
 * @param px The x-coordinate of the pixel.
 * @param py The y-coordinate of the pixel.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param rect The rectangle defining the area of the complex plane.
 * 
 * @return std::pair<long double, long double> The corresponding (x, y) coordinates in the complex plane.
 */
std::pair<long double, long double> pixel_to_xy(int px, int py, int width, int height, const Rectangle& rect) {
    long double x = rect.x_min + (rect.x_max - rect.x_min) * px / (long double)(width - 1);
    long double y = rect.y_min + (rect.y_max - rect.y_min) * py / (long double)(height - 1);
    return {x, y};
}

/**
 * @brief Maps the number of iterations to a color.
 * 
 * @param it The number of iterations before escape.
 * 
 * @return std::array<uint8_t,3> The BGR color corresponding to the iteration count.
 * 
 * @details This function uses a simple coloring algorithm to map iteration counts
 *          to colors. Points that do not escape (i.e., belong to the Mandelbrot set)
 *          are colored black. Other points are colored based on their iteration count
 *          using a gradient.
 */
static std::array<uint8_t,3> mandelbrot_colorize(int it){
    if(it >= MAX_ITERATIONS) return {0,0,0};
    // simple palette using a few bands
    double t = (double)it / (MAX_ITERATIONS - 1);
    uint8_t r = (uint8_t)std::round(9*(1-t)*t*t*t*255.0);   // red-ish
    uint8_t g = (uint8_t)std::round(15*(1-t)*(1-t)*t*t*255.0); // green-ish
    uint8_t b = (uint8_t)std::round(8.5*(1-t)*(1-t)*(1-t)*t*255.0); // blue-ish
    return {b,g,r};
}

/**
 * @brief Main function to generate the Mandelbrot set image.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * 
 * @return int Exit status.
 * 
 * @details The program expects five command-line arguments:
 *          1. x_min: Minimum x-coordinate of the rectangle.
 *          2. x_max: Maximum x-coordinate of the rectangle.
 *          3. y_min: Minimum y-coordinate of the rectangle.
 *          4. y_max: Maximum y-coordinate of the rectangle.
 *          5. out_file: Output BMP file name.
 */
int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(argc != 6) {
        std::cerr << "Usage: " << argv[0] << " x_min x_max y_min y_max out_file\n";
        return 1;
    }

    Rectangle rect;
    try{
        rect.x_min = std::stold(argv[1]);
        rect.x_max = std::stold(argv[2]);
        rect.y_min = std::stold(argv[3]);
        rect.y_max = std::stold(argv[4]);
    } catch(...) {
        std::cerr << "Error: Invalid rectangle coordinates.\n";
        return 1;
    }
    if(!(rect.x_min < rect.x_max && rect.y_min < rect.y_max)) {
        std::cerr << "Error: Invalid rectangle coordinates.\n";
        return 1;
    }

    std::string out = argv[5];

    long double x_range = rect.x_max - rect.x_min;
    long double y_range = rect.y_max - rect.y_min;
    
    HEIGHT = (int) std::llround(WIDTH * (y_range / x_range));
    if(HEIGHT <= 0) {
        std::cerr << "Error: Computed height is non-positive.\n";
        return 1;
    }

    // replace main's body after computing height:
    std::vector<uint8_t> bgr(WIDTH * HEIGHT * 3);

    auto idx = [&](int x, int y){ return (y * WIDTH + x) * 3; }; // BGR

    // Multithreaded render: manager enqueues rows, workers render rows.
    auto render_row = [&](int py){
        for(int px = 0; px < WIDTH; ++px){
            auto [x,y] = pixel_to_xy(px, py, WIDTH, HEIGHT, rect);
            int it = mandelbrot_iterations(x,y);
            // use color function
            auto c = mandelbrot_colorize(it);
            int p = idx(px,py);
            bgr[p+0] = c[0]; bgr[p+1] = c[1]; bgr[p+2] = c[2];
        }
    };

    JobQueue jq;

    // manager thread: push every row index
    std::thread manager([&]{
        for(int py = 0; py < HEIGHT; ++py) jq.push(py);
        jq.close();
    });

    // workers: at least 4 threads
    unsigned hw = std::max(4u, std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4u);
    std::vector<std::thread> workers;
    workers.reserve(hw);
    for(unsigned i = 0; i < hw; ++i){
        workers.emplace_back([&]{
            int row;
            while(jq.pop(row)){
                render_row(row);
            }
        });
    }

    manager.join();
    for(auto &t : workers) t.join();

    if(!write_bmp_24(out, WIDTH, HEIGHT, bgr)){
        std::cerr << "Error: cannot write " << out << std::endl;
        return 1;
    }
    return 0;
}