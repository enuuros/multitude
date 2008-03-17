#include "GPUFont.hpp"
#include "GlyphContainer.hpp"
#include "CPUFont.hpp"

namespace Dyslexic
{

  GPUFont::GPUFont(CPUFont * font)
  : m_cpuFont(font)
  {
    m_glyphList = new GlyphContainer(m_cpuFont->face());
  }

  GPUFont::~GPUFont()
  {
    delete m_glyphList;
  }

  bool GPUFont::checkGlyph(unsigned int charCode)
  {
    if(m_glyphList->glyph(charCode) == 0)
    {
      const Glyph * cpuGlyph = m_cpuFont->getGlyph(charCode);
      if(!cpuGlyph) return false;

      Glyph * gpuGlyph = makeGlyph(cpuGlyph);
      if(!gpuGlyph) return false;

      m_glyphList->add(gpuGlyph, charCode);
    }

    return true;
  }

  void GPUFont::faceSizeChanged()
  {
    delete m_glyphList;
    m_glyphList = new GlyphContainer(m_cpuFont->face());
  }

  void GPUFont::render(const char * str, const Nimble::Matrix3 & m)
  {
    internalRender(str, strlen(str), m);
  }

  void GPUFont::render(const wchar_t * str, const Nimble::Matrix3 & m)
  {
    internalRender(str, wcslen(str), m);
  }

  void GPUFont::internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform)
  {
    const wchar_t * c = str;
    m_pen.x = m_pen.y = 0.f;

    while(n--) {
      if(checkGlyph(*c)) {
        m_pen = m_glyphList->render(*c, *(c + 1), m_pen, transform);
      }
      c++;
    }
  }

  void GPUFont::internalRender(const char * str, int n, const Nimble::Matrix3 & transform)
  {
    const unsigned char * c = (unsigned char *)str;
    m_pen.x = m_pen.y = 0.f;

    while(n--) {
      if(checkGlyph(*c)) {
        m_pen = m_glyphList->render(*c, *(c + 1), m_pen, transform);
      }
      c++;
    }
  }

  void GPUFont::render(const std::string & str, const Nimble::Matrix3 & transform)
  {
    internalRender(str.c_str(), str.size(), transform);
  }

  void GPUFont::render(const std::string & str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str.c_str(), str.size(), transform);
  }

  void GPUFont::render(const char * str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str, strlen(str), transform);
  }

  void GPUFont::render(const wchar_t * str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str, wcslen(str), transform);
  }

  void GPUFont::render(const std::wstring & str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str.c_str(), str.length(), transform);
  }

  void GPUFont::render(const char * str, Nimble::Vector2 loc)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    transform[0][2] = loc.x;
    transform[1][2] = loc.y;
    internalRender(str, strlen(str), transform);    
  }

  void GPUFont::render(const std::wstring & str, const Nimble::Matrix3 & transform)
  {
    internalRender(str.c_str(), str.length(), transform);
  }

}

