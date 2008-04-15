#include "GPUManagedFont.hpp"
#include "CPUBitmapFont.hpp"
#include "CPUManagedFont.hpp"

#include <Nimble/Math.hpp>

namespace Dyslexic
{

  GPUManagedFont::GPUManagedFont(CPUManagedFont * cmf, Luminous::GLResources * glrc)
    : GLResource(glrc),
    m_cmf(cmf),
    m_resources(glrc)
  {
    m_fonts.resize(m_cmf->fontCount());
  }

  void GPUManagedFont::render(const std::string & text, int pointSize, const Nimble::Matrix3 & m)
  {
    float s = extractScale(m);

    float actual = pointSize * s;
    int fontNo = m_cmf->selectFont(actual);

    GPUFont * gf = getFont(fontNo);

    // Amount we need to scale
    const CPUFont * cf = m_cmf->getFont(fontNo);
    float sfix = actual / cf->faceSize();

    gf->render(text, m * Nimble::Matrix3::scale2D(Nimble::Vector2(sfix, sfix)));
  }

  void GPUManagedFont::render(const std::wstring & text, int pointSize, const Nimble::Matrix3 & m)
  {
    float s = extractScale(m);

    float actual = pointSize * s;
    int fontNo = m_cmf->selectFont(actual);

    GPUFontBase * font = getFont(fontNo);

    // Amount we need to scale
    float sfix = actual / m_cmf->getFont(fontNo)->faceSize();

    font->render(text, m * Nimble::Matrix3::scale2D(Nimble::Vector2(sfix, sfix)));
  }

  float GPUManagedFont::extractScale(const Nimble::Matrix3 & m)
  {
    Nimble::Vector3f u(1, 0, 0);
    Nimble::Vector3f v = m * u;
    float s = Nimble::Math::Sqrt(v.x * v.x + v.y * v.y);

    return s;
  }

  GPUFontBase * GPUManagedFont::getFont(int fontNo)
  {
    GPUFontBase * font = m_fonts[fontNo];
    if(!font) {
      // Create new
      CPUFontBase * cFont = dynamic_cast<CPUFontBase *> (m_cmf->getFont(fontNo));
      assert(cFont);
      font = new GPUTextureFont(cFont);

      m_fonts[fontNo] = font;
    }

    return font;
  }

}
