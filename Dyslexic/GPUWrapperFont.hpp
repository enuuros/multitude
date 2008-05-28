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
#ifndef DYSLEXIC_GPU_WRAPPER_FONT_HPP
#define DYSLEXIC_GPU_WRAPPER_FONT_HPP

#include <Dyslexic/GPUFont.hpp>
#include <Dyslexic/CPUWrapperFont.hpp>
#include <Dyslexic/GPUManagedFont.hpp>

#include <Luminous/GLResource.hpp>

namespace Dyslexic
{

  /// A wrapper class for convenience
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
