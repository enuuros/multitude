#ifndef DYSLEXIC_GPU_MANAGED_FONT_HPP
#define DYSLEXIC_GPU_MANAGED_FONT_HPP

#include <Luminous/GLResource.hpp>

#include <Nimble/Matrix3.hpp>

#include <vector>

namespace Dyslexic
{
  class GPUFontBase;
  class CPUManagedFont;

  class GPUManagedFont : public Luminous::GLResource
  {
    public:
      GPUManagedFont(CPUManagedFont * font, Luminous::GLResources * glrc);

      void render(const std::wstring & text, int pointSize, const Nimble::Matrix3 & m);
      void render(const std::string & text, int pointSize, const Nimble::Matrix3 & m);

    private:
      typedef std::vector<GPUFontBase *> container;

      float extractScale(const Nimble::Matrix3 & m);

      GPUFontBase * getFont(int fontNo);
      
      CPUManagedFont * m_cmf;  
      Luminous::GLResources * m_resources;
      container m_fonts;
  };


}

#endif
