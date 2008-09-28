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
#include "BBox.hpp"

#include <ft2build.h>
#include FT_OUTLINE_H

namespace Dyslexic
{

  BBox::BBox()
  : Nimble::Rect()
  {}

  BBox::BBox(FT_GlyphSlot glyph)
  {
    FT_BBox bbox;

    FT_Outline_Get_CBox(&glyph->outline, &bbox);
     
    low().x  = static_cast<float> (bbox.xMin) / 64.f;
    low().y  = static_cast<float> (bbox.yMin) / 64.f;
    high().x = static_cast<float> (bbox.xMax) / 64.f;
    high().y = static_cast<float> (bbox.yMax) / 64.f;
  }

}
