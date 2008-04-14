#ifndef DYSLEXIC_CPU_FONT_HPP
#define DYSLEXIC_CPU_FONT_HPP

#include <Dyslexic/GlyphContainer.hpp>
#include <Dyslexic/BBox.hpp>

#include <Luminous/Collectable.hpp>
#include <Luminous/GLResources.hpp>

namespace Dyslexic
{
  class CPUFont
  {
    public:
      virtual ~CPUFont() {}
  
      virtual float advance(const char * str) = 0;
      virtual float advance(const wchar_t * str) = 0;

      virtual int faceSize() const = 0;
      virtual bool setFaceSize(int size, int resolution) = 0;

      virtual float ascender() const = 0;
      virtual float descender() const = 0;
      virtual float lineHeight() const = 0;

      virtual void bbox(const char * str, BBox & bbox) = 0;
      virtual void bbox(const wchar_t * wstr, BBox & bbox) = 0;

      virtual bool load(const char * fontFilePath) = 0;
  };

}

#endif
