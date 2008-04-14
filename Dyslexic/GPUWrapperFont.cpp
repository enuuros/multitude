#include "GPUWrapperFont.hpp"

namespace Dyslexic
{

  GPUWrapperFont::GPUWrapperFont(GPUManagedFont * gmf, CPUWrapperFont * cf)
    : m_gmf(gmf),
    m_cf(cf)
  {}

  GPUWrapperFont::~GPUWrapperFont()
  {}

  CPUFont * GPUWrapperFont::cpuFont() 
  {
    return m_cf;
  }

  void GPUWrapperFont::internalRender(const char * str, int n, const Nimble::Matrix3 & transform)
  {
    m_gmf->render(std::string(str, n), m_cf->faceSize(), transform);
  }

  void GPUWrapperFont::internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform)
  {
    m_gmf->render(std::wstring(str, n), m_cf->faceSize(), transform);
  }

}

