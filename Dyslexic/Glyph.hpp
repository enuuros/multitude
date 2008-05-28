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
