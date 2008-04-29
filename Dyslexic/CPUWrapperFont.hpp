#ifndef DYSLEXIC_CPU_WRAPPER_FONT_HPP
#define DYSLEXIC_CPU_WRAPPER_FONT_HPP

#include <Dyslexic/CPUFont.hpp>

#include <Luminous/GLResources.hpp>

namespace Dyslexic
{
  class CPUManagedFont;
  class GPUWrapperFont;

  /// A font on the CPU that wraps the point size of the font for convenience.
  class CPUWrapperFont : public CPUFont
  {
    public:
      CPUWrapperFont(CPUManagedFont * mfont);
      ~CPUWrapperFont();

      float advance(const char * str);
      float advance(const wchar_t * str);      
    
      int faceSize() const        { return m_pointSize; }
      bool setFaceSize(int size, int = DYSLEXIC_DEFAULT_RESOLUTION)  { m_pointSize = size; return true; }

      float ascender() const;
      float descender() const;
      float lineHeight() const;

      void bbox(const char * str, BBox & bbox);
      void bbox(const wchar_t * str, BBox & bbox);

      bool load(const char * fontFilePath);

      GPUWrapperFont * getGPUFont(Luminous::GLResources * resources);

    protected:
      CPUManagedFont * m_managedFont;
      int m_pointSize;
  };


}

#endif
