#ifndef DYSLEXIC_CPU_BITMAP_FONT_HPP
#define DYSLEXIC_CPU_BITMAP_FONT_HPP

#include <GL/glew.h>
#include "CPUFont.hpp"
#include "GPUTextureFont.hpp"

namespace Dyslexic 
{

  /// A CPU font class that stores the glyphs as bitmaps.  
  class EXPORT CPUBitmapFont : public CPUFontBase
  {
  public:
    CPUBitmapFont(Luminous::GarbageCollector * gc = 0);
    virtual ~CPUBitmapFont();

    virtual bool setFaceSize(int size, int resolution = DYSLEXIC_DEFAULT_RESOLUTION);

    virtual GPUTextureFont * createGPUFont();

  private:
    inline virtual Glyph * makeGlyph(unsigned int glyph);
  };

}

#endif
