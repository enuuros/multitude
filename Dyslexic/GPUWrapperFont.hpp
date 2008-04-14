#ifndef DYSLEXIC_GPU_WRAPPER_FONT_HPP
#define DYSLEXIC_GPU_WRAPPER_FONT_HPP

#include <Dyslexic/GPUFont.hpp>
#include <Dyslexic/CPUWrapperFont.hpp>
#include <Dyslexic/GPUManagedFont.hpp>

#include <Luminous/GLResource.hpp>

namespace Dyslexic
{

  class GPUWrapperFont : public GPUFont, public Luminous::GLResource
  {
    public:
      GPUWrapperFont(GPUManagedFont * gf, CPUWrapperFont * cf);
      ~GPUWrapperFont();

      CPUFont * cpuFont();

    protected:
      void internalRender(const char * str, int n, const Nimble::Matrix3 & transform);
      void internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform);

      GPUManagedFont * m_gmf;
      CPUWrapperFont * m_cf;
  };

}

#endif
