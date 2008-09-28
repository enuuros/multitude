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
#ifndef DYSLEXIC_CPU_BITMAP_FONT_HPP
#define DYSLEXIC_CPU_BITMAP_FONT_HPP

#include <GL/glew.h>
#include "CPUFont.hpp"
#include "GPUTextureFont.hpp"

namespace Dyslexic 
{

  /// A CPU font class that stores the glyphs as bitmaps.  
  class MTEXPORT CPUBitmapFont : public CPUFontBase
  {
  public:
    CPUBitmapFont(Luminous::GarbageCollector * gc = 0);
    virtual ~CPUBitmapFont();

    virtual bool setFaceSize(int size, int resolution = DYSLEXIC_DEFAULT_RESOLUTION);

    virtual GPUTextureFont * createGPUFont();

  private:
    inline virtual Glyph * makeGlyph(unsigned int glyph);
  };

}

#endif
