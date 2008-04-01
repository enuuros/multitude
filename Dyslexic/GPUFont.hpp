#ifndef DYSLEXIC_GPU_FONT_HPP
#define DYSLEXIC_GPU_FONT_HPP

#include <Nimble/Matrix3.hpp>

#include <Luminous/GLResource.hpp>

namespace Dyslexic
{

  class CPUFont;
  class GlyphContainer;
  class Glyph;

  class GPUFont : public Luminous::GLResource
  {
    public:
      GPUFont(CPUFont * font);
      virtual ~GPUFont();

      CPUFont * cpuFont() { return m_cpuFont; }

      // Wrappers for usability, these just call the internalRender() functions
      void render(const char * str, const Nimble::Matrix3 & transform);
      void render(const wchar_t * str, const Nimble::Matrix3 & transform);

      void render(const std::string & str);
      void render(const std::string & str, const Nimble::Matrix3 & transform);

      void render(const std::wstring & str);
      void render(const std::wstring & str, const Nimble::Matrix3 & transform);

      void render(const char * str);
      void render(const wchar_t * str);

      void render(const char * str, Nimble::Vector2 loc);
      void render(const char * str, float scale, Nimble::Vector2 loc);

    protected:
      // The actual rendering methods, override these in derived classes
      virtual void internalRender(const char * str, int n, const Nimble::Matrix3 & transform);
      virtual void internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform);

      CPUFont * m_cpuFont;
      virtual Glyph * makeGlyph(const Glyph * cpuGlyph) = 0;

      virtual void faceSizeChanged();

    private:
      inline bool checkGlyph(unsigned int charCode);
      
      GlyphContainer * m_glyphList;
      Nimble::Vector2 m_pen;      

      friend class CPUFont;
  };

}

#endif
