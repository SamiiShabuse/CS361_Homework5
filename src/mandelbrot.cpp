#include <bits/stdc++.h>
#include "multithread.cpp"

#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

bool write_bmp_24(const std::string& filename, int width, int height, const std::vector<uint8_t>& bgr){
    // each row padded to multiple of 4 bytes
    int row_stride = ((width * 3 + 3) / 4) * 4;
    int img_size = row_stride * height;

    // headers
    uint8_t file_header[14] = {0};
    uint8_t info_header[40] = {0};

    // BITMAPFILEHEADER
    file_header[0] = 'B'; file_header[1] = 'M';
    uint32_t file_size = 14 + 40 + img_size;
    memcpy(&file_header[2], &file_size, 4);
    uint32_t pixel_offset = 14 + 40;
    memcpy(&file_header[10], &pixel_offset, 4);

    // BITMAPINFOHEADER
    uint32_t hdr_size = 40;
    memcpy(&info_header[0], &hdr_size, 4);
    memcpy(&info_header[4], &width, 4);
    memcpy(&info_header[8], &height, 4);
    uint16_t planes = 1, bpp = 24;
    memcpy(&info_header[12], &planes, 2);
    memcpy(&info_header[14], &bpp, 2);
    uint32_t compression = 0;
    memcpy(&info_header[16], &compression, 4);
    memcpy(&info_header[20], &img_size, 4);
    // DPI to pixels-per-meter
    int32_t ppm = 5906; // ~150 DPI
    memcpy(&info_header[24], &ppm, 4);
    memcpy(&info_header[28], &ppm, 4);
    uint32_t clr_used = 0, clr_imp = 0;
    memcpy(&info_header[32], &clr_used, 4);
    memcpy(&info_header[36], &clr_imp, 4);

    std::ofstream out(filename, std::ios::binary);
    if(!out) return false;
    out.write((char*)file_header, 14);
    out.write((char*)info_header, 40);

    // write bottom→top rows with padding
    int src_row_bytes = width * 3;
    std::vector<uint8_t> pad(row_stride - src_row_bytes, 0);

    for(int y = 0; y < height; ++y){
        int src_row = height - 1 - y; // BMP expects bottom row first
        const uint8_t* row = &bgr[src_row * src_row_bytes];
        out.write((const char*)row, src_row_bytes);
        if(!pad.empty()) out.write((const char*)pad.data(), pad.size());
    }
    return true;
}

static const int MAX_ITERATIONS = 1000;

struct Rectangle { long double x_min, x_max, y_min, y_max; };

static const int WIDTH = 1500;
int HEIGHT; // This will be determined by the number of points on the y axis.

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

std::pair<long double, long double> pixel_to_xy(int px, int py, int width, int height, const Rectangle& rect) {
    long double x = rect.x_min + (rect.x_max - rect.x_min) * px / (long double)(width - 1);
    long double y = rect.y_min + (rect.y_max - rect.y_min) * py / (long double)(height - 1);
    return {x, y};
}

static std::array<uint8_t,3> mandelbrot_colorize(int it){
    if(it >= MAX_ITERATIONS) return {0,0,0};
    // simple palette using a few bands
    double t = (double)it / (MAX_ITERATIONS - 1);
    uint8_t r = (uint8_t)std::round(9*(1-t)*t*t*t*255.0);   // red-ish
    uint8_t g = (uint8_t)std::round(15*(1-t)*(1-t)*t*t*255.0); // green-ish
    uint8_t b = (uint8_t)std::round(8.5*(1-t)*(1-t)*(1-t)*t*255.0); // blue-ish
    return {b,g,r};
}

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