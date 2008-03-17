#ifndef DYSLEXIC_GPU_TEXTURE_GLYPH_HPP
#define DYSLEXIC_GPU_TEXTURE_GLYPH_HPP

#include <GL/glew.h>
#include "Glyph.hpp"

namespace Dyslexic
{
  class CPUBitmapGlyph;

  class GPUTextureGlyph : public Glyph
  {
    public:
      GPUTextureGlyph(const CPUBitmapGlyph * glyph, int texId, int xOff, int yOff, GLsizei width, GLsizei height);
      virtual ~GPUTextureGlyph();

      virtual Nimble::Vector2 render(Nimble::Vector2 pen, const Nimble::Matrix3 & m);

      static void resetActiveTexture() { s_activeTexture = 0; }
    private:
      int m_width;
      int m_height;

      Nimble::Vector2 m_pos;
      Nimble::Vector2 m_uv[2];

      GLuint m_textureId;

      // Attempt to minimize texture changes
      static GLuint s_activeTexture;
  };

}

#endif
