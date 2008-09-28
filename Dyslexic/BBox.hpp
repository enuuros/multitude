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
