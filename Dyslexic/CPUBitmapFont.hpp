#ifndef DYSLEXIC_CPU_BITMAP_FONT_HPP
#define DYSLEXIC_CPU_BITMAP_FONT_HPP

#include <GL/glew.h>
#include "CPUFont.hpp"
#include "GPUTextureFont.hpp"

namespace Dyslexic 
{
  
  class CPUBitmapFont : public CPUFont
  {
  public:
    CPUBitmapFont(Luminous::GarbageCollector * gc = 0);
    virtual ~CPUBitmapFont();

    // virtual void render(const char * str);
    // virtual void render(const wchar_t * str);
  
    virtual bool setFaceSize(int size, int resolution);  

    virtual GPUTextureFont * createGPUFont();

  private:
    inline virtual Glyph * makeGlyph(unsigned int glyph);
  };

}

#endif
