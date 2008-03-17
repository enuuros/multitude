/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
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
#include <iostream>

using namespace std;

namespace Luminous
{

  typedef struct
  {
    unsigned char identSize;
    unsigned char colormapType;
    unsigned char imageType;
    
    unsigned char ignored[5]; 
    
    unsigned char xStartLo;
    unsigned char xStartHi;
    unsigned char yStartLo;
    unsigned char yStartHi;
  
    unsigned char widthLo;
    unsigned char widthHi;
    unsigned char heightLo;
    unsigned char heightHi;
    unsigned char bpp;
    unsigned char descriptor;
  } TGAHeader;

  bool Image::readTGA(FILE* file)
  {
    TGAHeader header;

    // Read header
    fread(&header, 1, sizeof(TGAHeader), file);

    // Check image type
    bool typeGood = false;

    switch(header.imageType) {
      case 2:   // RGB
      case 3:   // Grayscale
      case 10:  // RGB+RLE
      case 11:  // Grayscale+RLE
        typeGood = true;
    };  

    int width = header.widthLo + (header.widthHi << 8);
    int height = header.heightLo + (header.heightHi << 8);
    int bytesPerPixel = (header.bpp >> 3);

    if(!typeGood || width == 0 || height == 0) 
      return false;

    // Choose pixel format
    PixelFormat pf;
    switch(bytesPerPixel) {
      case 4:
        pf = PixelFormat(PixelFormat::LAYOUT_BGRA, PixelFormat::TYPE_UBYTE);
        break;
      case 3:
        pf = PixelFormat(PixelFormat::LAYOUT_BGR, PixelFormat::TYPE_UBYTE);
        break;
      case 1:
        pf = PixelFormat(PixelFormat::LAYOUT_LUMINANCE, PixelFormat::TYPE_UBYTE);
        break;
      default:
        cerr << "Image::readTGA # unsupported bit depth (" << header.bpp << ")" << endl;
        return false;
    };

    // Allocate memory
    allocate(width, height, pf);

    unsigned int size = m_width * m_height * bytesPerPixel;

    // Skip the ident field if present
    if(header.identSize > 0) 
      fseek(file, header.identSize, SEEK_CUR);

    if(header.imageType == 2 || header.imageType == 3) {

      // Uncompressed image
      fread( &m_data[0], size, 1, file);

    } else {

      // Compressed image
      int pixels = m_width * m_height;
      int currentPixel = 0;
      int currentByte = 0;
      unsigned char* pixel = new unsigned char [bytesPerPixel];      

      do {
        unsigned char chunkHeader = 0;
        fread(&chunkHeader, 1, 1, file);

        if(chunkHeader < 128) {
          chunkHeader++;

          for(int i = 0; i < chunkHeader; i++) { 
            fread(&m_data[currentByte], bytesPerPixel, 1, file);

            currentByte += bytesPerPixel;
            currentPixel++;
          }
        } else {
          chunkHeader -= 127;

          fread(pixel, 1, bytesPerPixel, file);

          for(int i = 0; i < chunkHeader; i++) {
            memcpy(&m_data[currentByte], pixel, bytesPerPixel);

            currentByte += bytesPerPixel;
            currentPixel++;
          }
        }
      } while(currentPixel < pixels);

      delete[] pixel;
    }

    return true;
  }


  bool Image::writeTGA(FILE* file)
  {
    // Fill the header
    TGAHeader header;
    memset(&header, 0, sizeof(TGAHeader));

    if(m_pixelFormat.type() != PixelFormat::TYPE_UBYTE) {
      cerr << "Image::writeTGA # can only write images with pixel data type UBYTE" << endl;
      return false;
    }

    bool reverse = false;

    switch(m_pixelFormat.layout()) {
      case PixelFormat::LAYOUT_RGB:
        reverse = true;
      case PixelFormat::LAYOUT_BGR:
        header.imageType = 2;
        break;
      case PixelFormat::LAYOUT_RGBA:
        reverse = true;
      case PixelFormat::LAYOUT_BGRA:
        header.imageType = 2;
        break;
      case PixelFormat::LAYOUT_LUMINANCE:
        header.imageType = 3;
        break;
      default:
        cerr << "Image::writeTGA # unsupported pixel layout" << endl;
        return false;
    };

    header.widthLo = (m_width & 0x00FF);
    header.widthHi = (m_width & 0xFF00);
    header.heightLo = (m_height & 0x00FF);
    header.heightHi = (m_height & 0xFF00);
    header.bpp = (m_pixelFormat.numChannels() << 3);

    // Write header
    fwrite(&header, sizeof(TGAHeader), 1, file);

    // Write data
    if(!reverse) {
      int size = m_width * m_height * m_pixelFormat.numChannels();
      fwrite( &m_data[0], 1, size, file);
    } else {
      
      int pixels = m_width * m_height;
      int bytesPerPixel = m_pixelFormat.numChannels();
      int currentPixel = 0;
      int currentByte = 0;

      unsigned char * pixel = new unsigned char [bytesPerPixel];

      do {
        
        memcpy(pixel, &m_data[currentByte], bytesPerPixel);
        
        pixel[0] = m_data[currentByte + 2];
        pixel[2] = m_data[currentByte + 0];

        fwrite(pixel, 1, bytesPerPixel, file);
    
        currentPixel++;
        currentByte += bytesPerPixel;
      
      } while(currentPixel < pixels);
    
      delete [] pixel;
    }

    return true;
  }

}
