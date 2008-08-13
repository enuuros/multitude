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

#include <Nimble/Math.hpp>

#include <Radiant/Trace.hpp>

#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cassert>
#include <cmath>

#ifdef WIN32
#include <strings.h>	// strcasecmp()
#include <WinPort.h>	// uint8_t
#endif

using namespace std;
using namespace Radiant;

namespace Luminous
{

  Image::Image()
  : m_width(0),
    m_height(0),
    m_pixelFormat(PixelFormat::LAYOUT_UNKNOWN, PixelFormat::TYPE_UNKNOWN),
    m_data(0)
  {}

  Image::Image(const Image& img)
  : m_width(0),
    m_height(0),
    m_pixelFormat(PixelFormat::LAYOUT_UNKNOWN, PixelFormat::TYPE_UNKNOWN),
    m_data(0)
  {
    allocate(img.m_width, img.m_height, img.m_pixelFormat);

    unsigned int bytes = m_width * m_height * m_pixelFormat.numChannels();
    memcpy(m_data, img.m_data, bytes);
  }

  Image::~Image()
  {
    clear();
  }

  void Image::flipVertical()
  {
    int linesize = m_width * m_pixelFormat.numChannels();

    int n = m_height / 2;

    for(int y = 0; y < n; y++) {
      uint8_t * l1 = line(y);
      uint8_t * l2 = line(m_height - y - 1);

      uint8_t * sentinel = l1 + linesize;

      while(l1 < sentinel) {
        uint8_t tmp = *l1;
        *l1 = *l2;
        *l2 = tmp;
        
        l1++;
        l2++;
      };
    }
  }

  bool Image::copyResample(const Image & source, int w, int h)
  {
    if(source.pixelFormat() == PixelFormat::rgbUByte()) {

      allocate(w, h, source.pixelFormat());

      int sw = source.width();
      int sh = source.height();

      float xscale = 0.5f * source.width()  / (float) w;
      float yscale = source.height() / (float) h;

      const uint8_t * src = source.bytes();
      uint8_t * dest = bytes();

      for(int y = 0; y < h; y++) {

	float sy = y * yscale;
	int yi = (int) sy;
	int yi1 = yi + 1;

	if(yi1 >= sh) yi1 = yi;
	float wy1 = sy - yi;
	float wy0 = 1.0f - wy1;

	for(int x = 0; x < w; x++) {

	  float sx = x * xscale;
	  int xi = (int) sx;
	  int xi1 = xi + 1;
	  if(xi1 >= sw) xi1 = xi;

	  float wx1 = sx - xi;
	  float wx0 = 1.0f - wx1;

	  const uint8_t * v00 = & src[(yi * sw  + xi) * 3];
          const uint8_t * v10 = src[(yi * sw  + xi1) * 3];
	  const uint8_t * v01 = src[(yi1 * sw + xi) * 3];
          const uint8_t * v11 = src[(yi1 * sw + xi1) * 3];
	  
	  float r = 
	    (*v00) * wy0 * wx0 + 
	    (*v10) * wy0 * wx1 + 
	    (*v01) * wy1 * wx0 + 
	    (*v11) * wy1 * wx1;

	  // *dest = Nimble::Math::Min((int) (interp + 0.5f), 255);

	  dest++;
	}
      }

      return true;
    }

    return false;
  }

  bool Image::quarterSize(const Image & source)
  {
    int sw = source.width();
    int w = sw / 2;
    /*if(sw & 0x1)
      w++;
    */
    int sh = source.height();
    int h = sh / 2;
    /*if(sh & 0x1)
      h++;
    */
    if(source.pixelFormat() == PixelFormat::rgbUByte()) {

      allocate(w + (sw & 0x1), h + (sh & 0x1), source.pixelFormat());

      const uint8_t * src = source.bytes();
      uint8_t * dest = bytes();
      
      for(int y = 0; y < h; y++) {

	const uint8_t * l0 = src + y * 2 * sw;
	const uint8_t * l1 = l0 + sw;

	for(int x = 0; x < w; x++) {
	  
	  unsigned tmp = l0[0];
	  tmp += l0[1];
	  tmp += l1[0];
	  tmp += l1[1];

	  *dest = tmp >> 2;

	  l0 += 2;
	  l1 += 2;
	
	  dest++;
	}

	// If we missed the last one, then:

	if(sw & 0x1) {
	  unsigned tmp = l0[0];
	  tmp += l1[0];

	  *dest = tmp >> 1;
	
	  dest++;	  
	}
	
      }

      if(sh & 0x1) {

	// Last line
	const uint8_t * l0 = src + ((h * 2) + 1) * sw;

	for(int x = 0; x < w; x++) {
	  
	  unsigned tmp = l0[0];
	  tmp += l0[1];

	  *dest = tmp >> 1;

	  l0 += 2;
	
	  dest++;
	}

	// Last pixel
	if(sw & 0x1) {
	  *dest = *l0;
	}
      }

      return true;
    }

    return false;
  }

  Image& Image::operator = (const Image& img) 
  {    
    allocate(img.m_width, img.m_height, img.m_pixelFormat);

    unsigned int bytes = m_width * m_height * m_pixelFormat.numChannels();
    memcpy(m_data, img.m_data, bytes);

    return *this;
  }

  void Image::allocate(int width, int height, const PixelFormat & pf)
  {
    unsigned int bytes = width * height * pf.numChannels();
    unsigned int mybytes = m_width * m_height * m_pixelFormat.numChannels();

    m_width = width;
    m_height = height;
    m_pixelFormat = pf;

    if(bytes != mybytes) {
      delete [] m_data;
      if(bytes)
	m_data = new unsigned char [bytes];
      else
	m_data = 0;
    }
  }

  bool Image::read(const char* filename, ImageType* pType)
  {
    clear();

    bool ret = false;

    // Guess the filetype from the extension
    string s(filename);

    string ext = s.substr(s.rfind(".") + 1);

    ImageType type = IMAGE_TYPE_UNKNOWN;

    if(strcasecmp(ext.c_str(), "tga") == 0) type = IMAGE_TYPE_TGA;
    else if(strcasecmp(ext.c_str(), "png") == 0) type = IMAGE_TYPE_PNG;
    else if(strcasecmp(ext.c_str(), "jpg") == 0 ||
        strcasecmp(ext.c_str(), "jpeg") == 0) type = IMAGE_TYPE_JPG;

    // Figure out the file type
    FILE* file = fopen(filename, "rb");
    if(file) {
      switch(type) {
        case IMAGE_TYPE_PNG:
          ret = readPNG(file);
          break;
        case IMAGE_TYPE_JPG:
          ret = readJPG(file);
          break;
        case IMAGE_TYPE_TGA:
          ret = readTGA(file);
          break;
        default:
          error("Image::read # unknown image format in '%s'", filename);
          break;
      }

    fclose(file);
  }

  if(pType) *pType = type;

  return ret;
  }

  bool Image::write(const char* filename, ImageType type)
  {
    bool ret = false;

    FILE* file = fopen(filename, "wb");
    if(!file) {
      error("Image::write # failed to open file '%s'", filename);
    } else {

      switch(type) {
        case IMAGE_TYPE_PNG:
          ret = writePNG(file);
          break;
        case IMAGE_TYPE_JPG:
          ret = writeJPG(file);
          break;
        default:
          error("Image::write # unknown file format");
          break;
      }

      fclose(file);
    }

    return ret;
  }

    void Image::fromData(const unsigned char * bytes, int width, int height,
		       PixelFormat format)
  {

    allocate(width, height, format);
    unsigned pixels = width * height;
    unsigned nbytes = pixels * format.numChannels();

    memcpy( & m_data[0], bytes, nbytes);
  }


  void Image::clear() 
  {
    delete[] m_data;
    m_data = 0;

    m_width = 0;
    m_height = 0;
    m_pixelFormat = PixelFormat(PixelFormat::LAYOUT_UNKNOWN,
        PixelFormat::TYPE_UNKNOWN);
  }
/*
  void Image::scale(int reqWidth, int reqHeight, bool keepAspectRatio, Image& dest) const
  {
    dest.clear();

    if(empty()) {
      trace("Image::scaled # Scaling empty image");
      return;
    }
#if 1
    dest = *this;
#else 
    // Compute new dimensions
    int newWidth, newHeight;

    if(!keepAspectRatio) {
      newWidth = reqWidth;
      newHeight = reqHeight;
    } else {
      int rw = reqHeight * m_width / m_height;

      bool useHeight = (rw <= reqWidth);

      if(useHeight) {
        newWidth = rw;
        newHeight = reqHeight;
      } else {
        newHeight = reqWidth * m_height / m_width;
        newWidth = reqWidth;
      }
    }

    // No need to scale, just return a copy
    if(newWidth == m_width && newHeight == m_height) {
      dest = *this;
      return;
    }

    dest.allocate(newWidth, newHeight, m_pixelFormat);

    float xRatio = (float)m_width / (float)newWidth;
    float yRatio = (float)m_height / (float)newHeight;

    for(int y = 0; y < newHeight; y++) {
      for(int x = 0; x < newWidth; x++) {

        float sx = (float)x * xRatio;
        float sy = (float)y * yRatio;

        sample(sx, sy, xRatio, yRatio, dest, x, y);
      }
    }
#endif
  }
*/    
  /// @todo currently ignores alpha channel
  void Image::sample(float x1, float y1, float x2, float y2, Image & dest, int destX, int destY) const
  {
    int begX = (int)x1;
    int begY = (int)y1;
    int endX = (int)x2;
    int endY = (int)y2;

    int nc = m_pixelFormat.numChannels();

    for(int y = begY; y < endY; y++) {
      for(int x = begX; x < endX; x++) {

        float w = computeWeight(x, y, x1, y1, x2, y2);

        assert(w > 0.0f && w <= 1.0f);

        unsigned int dstOffset = destY * dest.width() + destX;
        unsigned int srcOffset = y * m_width + x;
  
        for(int c = 0; c < nc; c++) 
          dest.m_data[nc * dstOffset + c] += (unsigned char)(w * m_data[nc * srcOffset + c]);
      }
    }
  }

  float Image::computeWeight(int x, int y, float x1, float y1, float x2, float y2) const
  {
    float sx = (x < x1 ? x1 : x);
    float sy = (y < y1 ? y1 : y);
    
    x++; y++;

    float ex = (x < x2 ? x : x2);
    float ey = (y < y2 ? y : y2);

    return (ex - sx) * (ey - sy);
  }

}
