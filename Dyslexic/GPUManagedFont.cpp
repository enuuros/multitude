/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#include "GPUManagedFont.hpp"
#include "CPUBitmapFont.hpp"
#include "CPUManagedFont.hpp"

#include <Nimble/Math.hpp>

//#include <Radiant/Trace.hpp>

namespace Dyslexic
{

  GPUManagedFont::GPUManagedFont(CPUManagedFont * cmf,
				 Luminous::GLResources * glrc)
    : GLResource(glrc),
    m_cmf(cmf),
    m_resources(glrc)
  {
    m_fonts.resize(m_cmf->fontCount());
  }

  void GPUManagedFont::render(const std::string & text,
			      int pointSize, const Nimble::Matrix3 & m)
  {
    GPUFont * gf;
    float sfix;

    computeRenderParams(m, pointSize, &gf, &sfix);
    gf->render(text, m * Nimble::Matrix3::scale2D(Nimble::Vector2(sfix, sfix)));
  }

  void GPUManagedFont::render(const std::wstring & text,
			      int pointSize, const Nimble::Matrix3 & m)
  {
    GPUFont * gf;
    float sfix;
    
    computeRenderParams(m, pointSize, &gf, &sfix);
    gf->render(text, m * Nimble::Matrix3::scale2D(Nimble::Vector2(sfix, sfix)));
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

  void GPUManagedFont::computeRenderParams(const Nimble::Matrix3 & m, int pts, GPUFont ** gf, float * scale)
  {
    float s = m.extractScale();

    float actual = pts * s;
    int fontNo = m_cmf->selectFont(actual);

    *gf = getFont(fontNo);

    // Amount we need to scale
    const CPUFont * cf = (*gf)->cpuFont();
    *scale = actual / (cf->faceSize() * s);

//    Radiant::trace("GPUManagedFont::render # (scale %f, font pts %d, actual %f); (used pts %d, scale fix %f, result %f)", s, pts, actual, cf->faceSize(), *scale, s * *scale * (float)cf->faceSize());
  }

}
