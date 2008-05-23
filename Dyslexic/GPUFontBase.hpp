#ifndef DYSLEXIC_GPU_FONT_BASE_HPP
#define DYSLEXIC_GPU_FONT_BASE_HPP

#include <Nimble/Matrix3.hpp>

#include <Luminous/GLResource.hpp>

#include <Dyslexic/GPUFont.hpp>
#include <Dyslexic/CPUFontBase.hpp>

namespace Dyslexic
{
  class GlyphContainer;
  class Glyph;

  /// A base class for different fonts that reside on the GPU
  class GPUFontBase : public GPUFont, public Luminous::GLResource
  {
    public:
      GPUFontBase(CPUFontBase * font);
      virtual ~GPUFontBase();

      CPUFont * cpuFont() { return dynamic_cast<CPUFont *> (m_cpuFont); }

    protected:
      CPUFontBase * m_cpuFont;

      virtual Glyph * makeGlyph(const Glyph * cpuGlyph) = 0;

      virtual void faceSizeChanged();

      virtual void internalRender(const char * str, int n, const Nimble::Matrix3 & transform);
      virtual void internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform);

    private:
      inline bool checkGlyph(unsigned int charCode);

      GlyphContainer * m_glyphList;
      Nimble::Vector2 m_pen;      

      friend class CPUFontBase;
  };

}

#endif