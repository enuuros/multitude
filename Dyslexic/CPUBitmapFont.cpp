#include "CPUBitmapFont.hpp"
#include "CPUBitmapGlyph.hpp"
#include "GPUTextureFont.hpp"

#include <Radiant/Trace.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Dyslexic
{

  CPUBitmapFont::CPUBitmapFont(Luminous::GarbageCollector * gc)
  : CPUFontBase(gc)
  {
  }

  CPUBitmapFont::~CPUBitmapFont()
  {
  }
  
  GPUTextureFont * CPUBitmapFont::createGPUFont()
  {
    return new GPUTextureFont(this);
  }

  Glyph * CPUBitmapFont::makeGlyph(unsigned int glyphIndex)
  {
    assert(m_face != 0);

    FT_GlyphSlot ftGlyph = m_face->glyph(glyphIndex, FT_LOAD_NO_HINTING);
    
    if(ftGlyph) {
        CPUBitmapGlyph * tempGlyph = new CPUBitmapGlyph(ftGlyph);
        return tempGlyph;
    }    

    m_error = m_face->error(); 

    return 0;
  }

  bool CPUBitmapFont::setFaceSize(int size, int resolution)
  {
    assert(m_face != 0);

    return CPUFontBase::setFaceSize(size, resolution);
  }

}

