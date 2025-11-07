#include <bits/stdc++.h>

static const int MAX_ITERATIONS = 1000;

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

std::pair<long double, long double> pixel_to_xy(int px, int py, const struct Rectangle& rect) {
    long double x = rect.x_min + (rect.x_max - rect.x_min) * px / (WIDTH - 1);
    long double y = rect.y_min + (rect.y_max - rect.y_min) * py / (HEIGHT - 1);
    return {x, y};
}


struct Rectangle { long double x_min, x_max, y_min, y_max; };

static const int WIDTH = 1500;
int HEIGHT; // This will be determined by the number of points on the y axis.


int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(argc != 6) {
        std::cerr << "Usage: " << argv[0] << " x_min x_max y_min y_max max_iterations\n";
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

    std::cout << "OK: " << WIDTH << " " << HEIGHT << "\n";
    return 0;
}