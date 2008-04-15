#ifndef DYSLEXIC_CPU_BITMAP_GLYPH_HPP
#define DYSLEXIC_CPU_BITMAP_GLYPH_HPP

#include <GL/glew.h>
#include "Glyph.hpp"

namespace Dyslexic
{

  /// A glyph that is stored as a bitmap on the CPU
  class CPUBitmapGlyph : public Glyph
  {
    public:
      CPUBitmapGlyph(FT_GlyphSlotRec_ * glyph);
      virtual ~CPUBitmapGlyph();

      virtual Nimble::Vector2 render(Nimble::Vector2 pen, const Nimble::Matrix3 & m);

    protected:
      Nimble::Vector2i m_size;
      unsigned char * m_bitmap;
      Nimble::Vector2 m_pos;

      friend class GPUTextureGlyph;
  };

}

#endif
