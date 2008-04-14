#ifndef DYSLEXIC_CPU_WRAPPER_FONT_HPP
#define DYSLEXIC_CPU_WRAPPER_FONT_HPP

#include <Dyslexic/CPUFont.hpp>

#include <Luminous/GLResources.hpp>

namespace Dyslexic
{
  class CPUManagedFont;
  class GPUWrapperFont;

  class CPUWrapperFont : public CPUFont
  {
    public:
      CPUWrapperFont(CPUManagedFont * mfont);
      ~CPUWrapperFont();

      float advance(const char * str);
      float advance(const wchar_t * str);      
    
      int faceSize() const        { return m_pointSize; }
      bool setFaceSize(int size, int )  { m_pointSize = size; return true; }

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
