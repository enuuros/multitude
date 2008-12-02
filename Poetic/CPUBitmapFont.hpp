/* COPYRIGHT
 *
 * This file is part of Poetic.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Poetic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef POETIC_CPU_BITMAP_FONT_HPP
#define POETIC_CPU_BITMAP_FONT_HPP

#include "CPUFont.hpp"
#include "GPUTextureFont.hpp"

#include <Poetic/Export.hpp>

#include <GL/glew.h>

namespace Poetic 
{

  /// A CPU font class that stores the glyphs as bitmaps.  
  class POETIC_API CPUBitmapFont : public CPUFontBase
  {
  public:
    CPUBitmapFont(Luminous::GarbageCollector * gc = 0);
    virtual ~CPUBitmapFont();

    virtual bool setFaceSize(int size, int resolution = POETIC_DEFAULT_RESOLUTION);

    virtual GPUTextureFont * createGPUFont();

  private:
    inline virtual Glyph * makeGlyph(unsigned int glyph);
  };

}

#endif
