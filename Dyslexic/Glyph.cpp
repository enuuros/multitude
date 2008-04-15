#include "Glyph.hpp"

#include <ft2build.h>
#include FT_GLYPH_H

namespace Dyslexic
{

  Glyph::Glyph(FT_GlyphSlotRec_ * glyph)
  : m_error(0)
  {
    if(glyph) {
      m_bbox = BBox(glyph);
      m_advance = Nimble::Vector2(glyph->advance.x / 64.f, glyph->advance.y / 64.f);
    }
  }

  Glyph::~Glyph()
  {}
  
}
