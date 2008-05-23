#include "CPUFontBase.hpp"
#include "GPUFontBase.hpp"

#include <Radiant/Trace.hpp>

namespace Dyslexic
{

  CPUFontBase::CPUFontBase(Luminous::GarbageCollector * gc)
    : Collectable(gc),
      m_face(0),
    m_glyphList(0)
  {
  }

  CPUFontBase::~CPUFontBase()
  {  
    delete m_glyphList;
    delete m_face;
  }

  void CPUFontBase::detach(GPUFontBase * gpuFont)
  {
    assert(gpuFont->cpuFont() == this);

    gpuFont->m_cpuFont = 0;

    for(container::iterator it = m_gpuFonts.begin();
	it != m_gpuFonts.end(); it++) {
      if(*it == gpuFont) { 
        m_gpuFonts.erase(it);
        return;
      }
    }
  
    assert(0);
  }

  bool CPUFontBase::setFaceSize(int size, int resolution)
  {
    if(!m_face) {
      Radiant::error("CPUFontBase::setSize # no font loaded yet!");
      return false;
    }

    m_size = m_face->size(size, resolution);
    m_error = m_face->error();

    if(m_error != 0)
      return false;

    if(m_glyphList != NULL)
      delete m_glyphList;

    m_glyphList = new GlyphContainer(m_face);

    // Notify all GPUFonts of the size change
    for(container::iterator it = m_gpuFonts.begin();
        it != m_gpuFonts.end(); it++) {
      (*it)->faceSizeChanged();
    }

    return true;
  }

  bool CPUFontBase::load(const char * fontFilePath)
  {
    m_face = new Face(fontFilePath);

     m_error = m_face->error();
     if(m_error == 0) {
      m_glyphList = new GlyphContainer(m_face);
      return true;
    }

    Radiant::error("CPUFontBase::load # loading failed (error code: %d)", m_error);
    return false;
  }

  int CPUFontBase::faceSize() const
  {
    return m_size.charSize();
  }

  float CPUFontBase::lineHeight() const
  {
    return m_size.charSize();
  }

  float CPUFontBase::ascender() const
  {
    return m_size.ascender();
  }

  float CPUFontBase::descender() const
  {
    return m_size.descender();
  }

  /// @todo check the bbox calculations, there seems to be some error here
  void CPUFontBase::bbox(const char * str, BBox & bbox)
  {
    if(str && (*str != '\0')) {
        const unsigned char * c = (unsigned char *)str;
        float advance = 0.f;
        if(checkGlyph(*c)) {
            bbox = m_glyphList->bbox(*c); 
            advance = m_glyphList->advance(*c, *(c + 1));
        }

        while(*++c) {
          if(checkGlyph(*c)) {
             BBox tempBBox = m_glyphList->bbox(*c);
                tempBBox.move(Nimble::Vector2(advance, 0.f));
                bbox.expand(tempBBox);
                advance += m_glyphList->advance(*c, *(c + 1));
            }
        }
    }
  }

  void CPUFontBase::bbox(const wchar_t * wstr, BBox & bbox)
  {
    if(wstr && (*wstr != wchar_t('\0')))
    {
      const unsigned wchar_t *   wc = (unsigned wchar_t *)wstr;
      float   advance = 0.f;
      if(checkGlyph(*wc))
      {
        bbox = m_glyphList->bbox(*wc); 
        advance = m_glyphList->advance(*wc, *(wc + 1));
      }

      while(*++wc)
      {
        if(checkGlyph(*wc))
        {
          BBox tempBBox = m_glyphList->bbox(*wc);
          tempBBox.move(Nimble::Vector2(advance, 0.0f));
          bbox.expand(tempBBox);
          advance += m_glyphList->advance(*wc, *(wc + 1));
        }
      }
    }
  }

  bool CPUFontBase::checkGlyph(unsigned int characterCode)
  {
    if(m_glyphList->glyph(characterCode) == 0)
    {
        unsigned int glyphIndex = m_glyphList->fontIndex(characterCode);
        Glyph * tempGlyph = makeGlyph(glyphIndex);
        if(tempGlyph == 0) {
            if(m_error == 0)
                m_error = 0x13;

            return false;
        }
        
        m_glyphList->add(tempGlyph, characterCode);
    }

    return true;
  }

  float CPUFontBase::advance(const char * str)
  {
    const unsigned char * c = (unsigned char *)str;
    float width = 0.f;

    while(*c) {
      if(checkGlyph(*c)) {
        width += m_glyphList->advance(*c, *(c + 1));
      }
      ++c;
    }

    return width;
  }

  float CPUFontBase::advance(const wchar_t * str)
  {
    const wchar_t * c = str;
    float width = 0.f;

    while(*c) {
      if(checkGlyph(*c)) {
        width += m_glyphList->advance(*c, *(c + 1));
      }
      ++c;
    }

    return width;
  }

  const Glyph * CPUFontBase::getGlyph(unsigned int charCode)
  {
    // Guard guard(m_mutex);
    if(checkGlyph(charCode))
      return m_glyphList->glyph(charCode);
    
    return 0;
  }

  GPUFont * CPUFontBase::getGPUFont(Luminous::GLResources * resources) 
  {
    Luminous::GLResource * gf = resources->getResource(this);

    if(gf) {
      GPUFont * font = dynamic_cast<GPUFont *> (gf);
      assert(font);
      return font;
    }

    // puts("CPUFontBase::getGPUFont # New GPU font");
    
    GPUFontBase * font = createGPUFont();
    assert(font != 0);
    
    resources->addResource(this, font);

    m_gpuFonts.push_back(font);
    
    return font;
  }

}
