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

#ifndef LUMINOUS_IMAGE_HPP
#define LUMINOUS_IMAGE_HPP

#include <Luminous/PixelFormat.hpp>
#include <cstdio>

#include <vector>

namespace Luminous
{

  /// Simple image handling class
  class Image
  {
  public:
    enum ImageType {
      IMAGE_TYPE_UNKNOWN,
      IMAGE_TYPE_PNG,
      IMAGE_TYPE_JPG,
      IMAGE_TYPE_TGA
    };

    Image();
    Image(const Image& img);
    ~Image();

    float aspect() const { return (float)m_width / (float)m_height; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    int lineSize() { return m_width * m_pixelFormat.numChannels(); }

    unsigned char* line(unsigned y) { return &m_data[y * lineSize()]; }

    unsigned char* bytes() { return & m_data[0]; }
    const unsigned char* bytes() const { return & m_data[0]; }

    bool read(const char* filename, ImageType* type = 0);
    bool write(const char* filename, ImageType type);

    void fromData(const unsigned char * bytes, int width, int height, 
		  PixelFormat format);

    const PixelFormat& pixelFormat() const { return m_pixelFormat; }

    void allocate(int width, int height, const PixelFormat& pf);     
    void clear();

    bool empty() const { return (m_data == 0); }

    void flipVertical();

    /** Resample a source image using straightforward bilinear
	interpolation. */
    bool copyResample(const Image & source, int w, int h);

    bool quarterSize(const Image & source);

    bool forgetLastPixels(int n);
    void forgetLastLine();

    bool hasAlpha() const;

    Image & operator = (const Image& img);

  protected:

    void sample(float x1, float y1, float x2, float y2, Image & dest, int destX, int destY) const;
    float computeWeight(int x, int y, float x1, float y1, float x2, float y2) const;
  
    bool readPNG(FILE* file);
    bool writePNG(FILE* file);

    bool readJPG(FILE* file);
    bool writeJPG(FILE* file);

    bool readTGA(FILE* file);
    bool writeTGA(FILE* file);

    int m_width;
    int m_height;
    PixelFormat m_pixelFormat;
    unsigned char* m_data;    
  };

}

#endif
