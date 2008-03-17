#ifndef DYSLEXIC_CPU_FONT_HPP
#define DYSLEXIC_CPU_FONT_HPP

#include "BBox.hpp"
#include "GlyphContainer.hpp"

#include <Luminous/Collectable.hpp>
#include <Luminous/GLResources.hpp>

namespace Dyslexic
{
  class GPUFont;

  class CPUFont : public Luminous::Collectable
  {
  public:
      CPUFont(Luminous::GarbageCollector * gc = 0);
      virtual ~CPUFont();

      float advance(const char * str);
      float advance(const wchar_t * str);

      void detach(GPUFont * gpuFont);
      
      const Glyph * getGlyph(unsigned int charCode);

      virtual bool load(const char * fontFilePath);

      int faceSize() const;
      virtual bool setFaceSize(int size, int resolution);

      float ascender() const;
      float descender() const;      
      float lineHeight() const;

      Face * face() { return m_face; }
      const Size & size() const { return m_size; }

      void bbox(const char * str, BBox & bbox);
      void bbox(const wchar_t * wstr, BBox & bbox);

      int error() const { return m_error; }

      virtual GPUFont * createGPUFont() = 0;
      GPUFont * getGPUFont(Luminous::GLResources * resources);

    protected:
      virtual Glyph * makeGlyph(unsigned int g) = 0;

      Face * m_face;
      Size m_size;
      int m_error;

    private:
      inline bool checkGlyph(unsigned int g);
      
      GlyphContainer * m_glyphList;
      Nimble::Vector2 m_pen;
  
      std::vector<GPUFont *> m_gpuFonts;

      friend class GPUFont;
  };

}

#endif
