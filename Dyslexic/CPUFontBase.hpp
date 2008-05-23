#ifndef DYSLEXIC_CPU_FONT_BASE_HPP
#define DYSLEXIC_CPU_FONT_BASE_HPP

#include <Dyslexic/CPUFont.hpp>

#include <Luminous/Collectable.hpp>

namespace Dyslexic
{
  class GPUFontBase;
  class GPUFont;

  /// A base class for all CPU fonts.
  class MTEXPORT CPUFontBase : public CPUFont, public Luminous::Collectable
  {
    public:
      CPUFontBase(Luminous::GarbageCollector * gc = 0);
      virtual ~CPUFontBase();

      float advance(const char * str);
      float advance(const wchar_t * str);

      int faceSize() const;
      virtual bool setFaceSize(int size, int resolution);

      float ascender() const;
      float descender() const;      
      float lineHeight() const;

      void bbox(const char * str, BBox & bbox);
      void bbox(const wchar_t * wstr, BBox & bbox);

    ////
      void detach(GPUFontBase * gpuFont);

      const Glyph * getGlyph(unsigned int charCode);

      virtual bool load(const char * fontFilePath);

      Face * face() { return m_face; }
      const Size & size() const { return m_size; }

      int error() const { return m_error; }

      virtual GPUFontBase * createGPUFont() = 0;
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

      typedef std::vector<GPUFontBase *> container;  
      container m_gpuFonts;

      friend class GPUFont;

  };

}

#endif
