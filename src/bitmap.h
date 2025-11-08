/**
 * @file src/bitmap.h
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

#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>
#include <string>
#include <vector>

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
bool write_bmp_24(const std::string& filename, int width, int height, const std::vector<uint8_t>& bgr);

#endif // BITMAP_H
