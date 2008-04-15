#ifndef DYSLEXIC_GLYPH_HPP
#define DYSLEXIC_GLYPH_HPP

#include "BBox.hpp"

#include <Nimble/Vector2.hpp>

struct FT_GlyphSlotRec_;

namespace Dyslexic
{
  /// The base class for Glyphs.
  /// Glyph provides an interface between FreeType glyphs and our own.
  class Glyph
  {
    public:
      Glyph(FT_GlyphSlotRec_ * glyph);
      virtual ~Glyph();

      virtual Nimble::Vector2 render(Nimble::Vector2 pen, const Nimble::Matrix3 & m) = 0;

      Nimble::Vector2 advance() const { return m_advance; }
      const BBox & bbox() const { return m_bbox; }
      int error() const { return m_error; }

    protected:
      Nimble::Vector2 m_advance;
      BBox m_bbox;
      int m_error;
  };

}

#endif
