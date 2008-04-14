#ifndef DYSLEXIC_GPU_TEXTURE_FONT_HPP
#define DYSLEXIC_GPU_TEXTURE_FONT_HPP

#include <GL/glew.h>

#include <Dyslexic/GPUFontBase.hpp>

#include <vector>

namespace Dyslexic 
{

  class GPUTextureFont : public GPUFontBase
  {
  public:
    GPUTextureFont(CPUFontBase * cpuFont);
    virtual ~GPUTextureFont();

  protected:
    virtual void internalRender(const char * str, int n, const Nimble::Matrix3 & m);
    virtual void internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & m);
    
    inline virtual Glyph * makeGlyph(const Glyph * cpuGlyph);

    virtual void faceSizeChanged();

  private:
    inline void calculateTextureSize();
    inline GLuint createTexture();

    void resetGLResources();
   
    GLsizei m_maxTextureSize;

    GLsizei m_texWidth;
    GLsizei m_texHeight;

    std::vector<GLuint> m_textures;

    int m_glyphMaxWidth;
    int m_glyphMaxHeight;

    int m_padding;

    unsigned int m_numGlyphs;
    unsigned int m_remGlyphs;

    int m_xOffset;
    int m_yOffset;

    bool m_reset;
  };

}

#endif
