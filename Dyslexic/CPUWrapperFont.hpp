/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DYSLEXIC_CPU_WRAPPER_FONT_HPP
#define DYSLEXIC_CPU_WRAPPER_FONT_HPP

#include <Dyslexic/CPUFont.hpp>
#include <Dyslexic/Export.hpp>

#include <Luminous/GLResources.hpp>

namespace Dyslexic
{
  class CPUManagedFont;
  class GPUWrapperFont;

  /// A font on the CPU that wraps the point size of the font for convenience.
  class DYSLEXIC_API CPUWrapperFont : public CPUFont
  {
  public:
    CPUWrapperFont(CPUManagedFont * mfont);
    ~CPUWrapperFont();

    float advance(const char * str, int n = -1);
    float advance(const wchar_t * str, int n = -1);
    
    int faceSize() const        { return m_pointSize; }
    bool setFaceSize(int size, int = DYSLEXIC_DEFAULT_RESOLUTION)
    { m_pointSize = size; return true; }

    float minimumRenderSize() const        { return m_minimumRenderSize; }
    void setMinimumRenderSize(float size) { m_minimumRenderSize = size; }

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
    float m_minimumRenderSize;
  };


}

#endif
