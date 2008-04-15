#ifndef DYSLEXIC_BBOX_HPP
#define DYSLEXIC_BBOX_HPP

#include <Nimble/Rect.hpp>

struct FT_GlyphSlotRec_;

namespace Dyslexic
{

  /// A bounding box class for glyphs.
  class BBox : public Nimble::Rect
  {
    public:
      BBox();
      BBox(FT_GlyphSlotRec_ * glyph);

  };

}

#endif
