/**
 * @file src/bitmap.cpp
 * 
 * @brief Provides functions for creating and saving BMP images.
 * 
 * @author Samii Shabuse <sus24@drexel.edu>
 * @date November 7, 2025
 * 
 * @section Overview
 * 
 * This file contains functions for creating and saving BMP images.
 */

#include "bitmap.h"
#include <fstream>
#include <cstring>

/**
 * @brief Writes a 24-bit BMP image to a file.
 * 
 * @param filename The name of the output BMP file.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param bgr A vector containing the pixel data in BGR format.
 * 
 * @return true if the file was written successfully, false otherwise.
 * 
 * @details The function constructs the BMP file and info headers, handles
 *          row padding, and writes the pixel data in bottom-to-top order
 *          as required by the BMP format.
 */
bool write_bmp_24(const std::string& filename, int width, int height, const std::vector<uint8_t>& bgr){
    // each row padded to multiple of 4 bytes
    int row_stride = ((width * 3 + 3) / 4) * 4;
    int img_size = row_stride * height;

    // headers
    uint8_t file_header[14] = {0};
    uint8_t info_header[40] = {0};

    // Bitmap File Header
    file_header[0] = 'B'; file_header[1] = 'M';
    uint32_t file_size = 14 + 40 + img_size;
    memcpy(&file_header[2], &file_size, 4);
    uint32_t pixel_offset = 14 + 40;
    memcpy(&file_header[10], &pixel_offset, 4);

    // Bitmap Info Header
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
    int32_t ppm = 5906; // 150ish DPI
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
