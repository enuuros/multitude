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
     
    m_low.x  = static_cast<float> (bbox.xMin) / 64.f;
    m_low.y  = static_cast<float> (bbox.yMin) / 64.f;
    m_high.x = static_cast<float> (bbox.xMax) / 64.f;
    m_high.y = static_cast<float> (bbox.yMax) / 64.f;
  }

}
