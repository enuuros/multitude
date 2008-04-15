#ifndef DYSLEXIC_GLYPH_CONTAINER_HPP
#define DYSLEXIC_GLYPH_CONTAINER_HPP

#include "Face.hpp"
#include "Glyph.hpp"
#include "Charmap.hpp"
#include <vector>

namespace Dyslexic
{

  /// A container class for glyphs.
  class GlyphContainer
  {
    public:
      GlyphContainer(Face * face);
      ~GlyphContainer();

      void add(Glyph * glyph, unsigned int characterCode);
      float advance(unsigned int characterCode, unsigned int nextCharacterCode);
      BBox bbox(unsigned int characterCode) const;
      bool charMap(int encoding);
      
      unsigned int fontIndex(unsigned int characterCode) const;

      const Glyph * glyph(unsigned int characterCode) const;

      Nimble::Vector2 render(unsigned int charCode, unsigned int nextCharCode, Nimble::Vector2 penPos, const Nimble::Matrix3 & m);

    private:
      Face * m_face;
      std::vector<Glyph *> m_glyphs;
      int m_error;
      Charmap * m_charmap;
  };

}

#endif
