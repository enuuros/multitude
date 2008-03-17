#ifndef DYSLEXIC_BBOX_HPP
#define DYSLEXIC_BBOX_HPP

#include <Nimble/Rect.hpp>

struct FT_GlyphSlotRec_;

namespace Dyslexic
{

  class BBox : public Nimble::Rect
  {
    public:
      BBox();
      BBox(FT_GlyphSlotRec_ * glyph);

  };

}

#endif
