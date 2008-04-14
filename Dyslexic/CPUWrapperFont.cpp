#include "CPUWrapperFont.hpp"
#include "GPUWrapperFont.hpp"
#include "CPUManagedFont.hpp"

#include <Radiant/Trace.hpp>

namespace Dyslexic
{

  CPUWrapperFont::CPUWrapperFont(CPUManagedFont * mfont)
  : m_managedFont(mfont),
  m_pointSize(16)
  {}

  CPUWrapperFont::~CPUWrapperFont()
  {}

  GPUWrapperFont * CPUWrapperFont::getGPUFont(Luminous::GLResources * resources)
  {
    GPUWrapperFont * gf = dynamic_cast<GPUWrapperFont *> (resources->getResource(this));

    if(gf) 
      return gf;

    // Get the managed GPU font
    GPUManagedFont * gmf = dynamic_cast<GPUManagedFont *> (resources->getResource(m_managedFont));
    if(!gmf) {
      gmf = new GPUManagedFont(m_managedFont, resources);
      resources->addResource(m_managedFont, gmf);
    }

    // Create the resource                
    GPUWrapperFont * font = new GPUWrapperFont(gmf, this);
    resources->addResource(this, font);

    return font;
  }

  float CPUWrapperFont::advance(const char * str)
  {
    CPUFont * f = m_managedFont->getMetricFont();
//    float s = static_cast<float> (f->faceSize()) / static_cast<float> (m_pointSize);
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    return f->advance(str) * s;
  }

  float CPUWrapperFont::advance(const wchar_t * str)
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    return f->advance(str) * s;
  }
  
  float CPUWrapperFont::ascender() const 
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    return f->ascender() * s;
  }

  float CPUWrapperFont::descender() const 
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    return f->descender() * s;
  }

  float CPUWrapperFont::lineHeight() const 
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    return f->lineHeight() * s;
  }

  void CPUWrapperFont::bbox(const char * str, BBox & bbox)
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    f->bbox(str, bbox);
    bbox.scale(s);
  }

  void CPUWrapperFont::bbox(const wchar_t * str, BBox & bbox)
  {
    CPUFont * f = m_managedFont->getMetricFont();
    float s = static_cast<float> (m_pointSize) / static_cast<float> (f->faceSize());

    f->bbox(str, bbox);
    bbox.scale(s);
  }

  bool CPUWrapperFont::load(const char * )
  {
    Radiant::error("CPUWrapperFont::load # don't call me");
    return false;
  }

}

