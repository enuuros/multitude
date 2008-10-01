/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/Image.hpp>
#include <png/png.h>
#include <iostream>

using namespace std;

namespace Luminous
{
  
  bool Image::readPNG(FILE* file)
  {
    // Initialize IO stuff
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
    if(png_ptr == NULL) {
        cerr << "Image::readPNG # couldn't create PNG read struct" << endl;
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      cerr << "Image::readPNG # couldn't create PNG info struct" << endl;
      return false;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      cerr << "Image::readPNG # couldn't set png_jumpbuf" << endl;
      return false;
    }

    png_init_io(png_ptr, file);
    png_read_info(png_ptr, info_ptr);

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);

    // Convert palette color to RGB
    if(color_type == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(png_ptr);

    // Convert grayscale with less than 8 bpp to 8 bpp
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
      png_set_gray_1_2_4_to_8(png_ptr);

    // Add full alpha channel if there's transparency
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(png_ptr);

    // PNGs support 16 bpp, we don't
    if(bit_depth == 16) {
      cout << "Image::readPNG # warning, converting 16-bit channels to 8-bit" << endl;
      png_set_strip_16(png_ptr);
    }

    // If there's more than one pixel per byte, expand to 1 pixel / byte
    if(bit_depth < 8)
      png_set_packing(png_ptr);
  
    png_read_update_info(png_ptr, info_ptr);

    int width      = png_get_image_width(png_ptr, info_ptr);
    int height     = png_get_image_height(png_ptr, info_ptr);
    int channels = png_get_channels(png_ptr, info_ptr);
    int rowsize  = width * channels;

    PixelFormat pf;
    switch(channels) {
      case 4:
        pf = PixelFormat(PixelFormat::LAYOUT_RGBA, PixelFormat::TYPE_UBYTE);
        break;
      case 3:
        pf = PixelFormat(PixelFormat::LAYOUT_RGB, PixelFormat::TYPE_UBYTE);
        break;
      case 2:
        pf = PixelFormat(PixelFormat::LAYOUT_LUMINANCE_ALPHA, PixelFormat::TYPE_UBYTE);
        break;
      case 1:
        pf = PixelFormat(PixelFormat::LAYOUT_LUMINANCE, PixelFormat::TYPE_UBYTE);  
        break;
      default:
        cerr << "Image::readPNG # unsupported number of channels (" << channels << ") found" << endl;
        return false; 
    };

    // Allocate memory
    allocate(width, height, pf);

    png_bytep* row_pointers = new png_bytep [m_height];

    for(int i = 0; i < m_height; i++)
      row_pointers[i] = &m_data[0] + i * rowsize;
  
    png_read_image(png_ptr, row_pointers);

    delete[] row_pointers;

    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    return true;
  }

  bool Image::writePNG(FILE* file) 
  {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL, png_error_ptr_NULL, png_error_ptr_NULL);
    if(png_ptr == NULL) {
      cerr << "Image::writePNG # couldn't create a PNG write struct" << endl;
      return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
      cerr << "Image::writePNG # couldn't create a PNG info struct" << endl;
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
      cerr << "Image::writePNG # png_jmpbuf failed" << endl;
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return false;
    }

    png_init_io(png_ptr, file);

    int color_type;
    int channels = m_pixelFormat.numChannels();

    switch(channels) {
      case 1:
        color_type = PNG_COLOR_TYPE_GRAY;
        break;
      case 2:
        color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
        break;
      case 3:
        color_type = PNG_COLOR_TYPE_RGB;
        break;
      case 4:
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        break;
      default:
        cerr << "Image::writePNG # cannot write a PNG file with " << channels << " channels." << endl;
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    };

    png_set_IHDR(png_ptr, info_ptr,
        m_width, m_height,
        8, // bit depth
        color_type,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    int rowsize = m_width * channels;
    unsigned char** row_pointers = new unsigned char* [m_height];

    for(int i = 0; i < m_height; i++)
      row_pointers[i] = &m_data[0] + rowsize * i;

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    delete[] row_pointers;
  
    png_destroy_write_struct(&png_ptr, &info_ptr);

    return true;
  }

}

