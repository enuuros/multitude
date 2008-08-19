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
#ifndef DYSLEXIC_GPU_MANAGED_FONT_HPP
#define DYSLEXIC_GPU_MANAGED_FONT_HPP

#include <Luminous/GLResource.hpp>

#include <Nimble/Matrix3.hpp>

#include <vector>

namespace Dyslexic
{
  class GPUFontBase;
  class CPUManagedFont;
  class GPUFont;

  /// A managed font that contains multiple sizes of a single font residing on
  /// the GPU
  class GPUManagedFont : public Luminous::GLResource
  {
    public:
      GPUManagedFont(CPUManagedFont * font, Luminous::GLResources * glrc);

      void render(const std::string & text, int pointSize,
                  const Nimble::Matrix3 & m);
      void render(const char * str, int n, int pointSize,
                  const Nimble::Matrix3 & m);

      void render(const std::wstring & text, int pointSize,
                  const Nimble::Matrix3 & m);
      void render(const wchar_t * str, int n, int pointSize,
                  const Nimble::Matrix3 & m);


    private:
      typedef std::vector<GPUFontBase *> container;

      void computeRenderParams(const Nimble::Matrix3 & m, int pts, GPUFont ** gf, float * scale);

      GPUFontBase * getFont(int fontNo);
      
      CPUManagedFont * m_cmf;  
      Luminous::GLResources * m_resources;
      container m_fonts;
  };


}

#endif
