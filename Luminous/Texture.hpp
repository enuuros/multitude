/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef LUMINOUS_TEXTURE_HPP
#define LUMINOUS_TEXTURE_HPP

#include <GL/glew.h>
#include <Magick++.h>
#include "Image.hpp"
#include "GLResource.hpp"
#include <Patterns/NotCopyable.hpp>

namespace Luminous
{
  class PixelFormat;

  template<GLenum TextureType>
  class TextureT : public GLResource, public Patterns::NotCopyable
  {
    friend class Framebuffer;

  public:
    TextureT(GLResources * res = 0) 
    : GLResource(res),
      m_textureId(0),
      m_width(0),
      m_height(0),
      m_pf(PixelFormat::LAYOUT_UNKNOWN, PixelFormat::TYPE_UNKNOWN),
      m_haveMipmaps(false)
    {}
    virtual ~TextureT() { glDeleteTextures(1, &m_textureId); }

    void allocate()
    {
      if(!m_textureId)
        glGenTextures(1, &m_textureId);
    }

    /** Activate textureUnit and bind this texture to that unit. */
    void bind(GLenum textureUnit)
    {
      allocate();
      glActiveTexture(textureUnit);
      glBindTexture(TextureType, m_textureId);
    }

    /** Bind this texture to the currently active tecture unit. */
    void bind()
    {
      allocate();
      glBindTexture(TextureType, m_textureId);
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    
    void setWidth(int w) { m_width = w; }
    void setHeight(int h) { m_height = h; }

    /** Returns the aspect ratio of this texture. This operation makes
      sense mostly for 2D tetures.  */
    float aspectRatio() 
    { return m_height ? m_width / (float) m_height : 1.0f; }

    virtual long consumesBytes() 
    { 
      float used = m_width * m_height * m_pf.bytesPerPixel();
      // Mipmaps used 33% more memory
      used *= (m_haveMipmaps ? (4.f / 3.f) : 1.f);
      return (long)used;
    }

    /** Returns the OpenGL texture id. */
    GLuint id() const { return m_textureId; };

    const PixelFormat & pixelFormat() const { return m_pf; }

  protected:
    GLuint m_textureId;
    int m_width;
    int m_height;
    PixelFormat m_pf;
    bool m_haveMipmaps;
  };
  
  class Texture1D : public TextureT<GL_TEXTURE_1D>
  {
  public:
    Texture1D(GLResources * resources = 0) : TextureT<GL_TEXTURE_1D> (resources) {}

    static Texture1D* fromImage(Magick::Image & image, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture1D* fromBytes(GLenum internalFormat, int w, const void* data, const PixelFormat& srcFormat, bool buildMipmaps = true, GLResources * resources = 0);
  };

  class Texture2D : public TextureT<GL_TEXTURE_2D>
  {
  public:
    Texture2D(GLResources * resources = 0) : TextureT<GL_TEXTURE_2D>(resources) {}

    bool loadImage(Luminous::Image & image, bool buildMipmaps = true);
    bool loadImage(Magick::Image & image, bool buildMipmaps = true);
    bool loadBytes(GLenum internalFormat, int w, int h,
		   const void* data, 
		   const PixelFormat& srcFormat,
		   bool buildMipmaps = true);

    static Texture2D * fromImage(Luminous::Image & img, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture2D * fromImage(Magick::Image& image, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture2D * fromBytes(GLenum internalFormat, int w, int h,
				const void * data, 
				const PixelFormat& srcFormat,
				bool buildMipmaps = true, GLResources * resources = 0);
  };

  class Texture3D : public TextureT<GL_TEXTURE_3D>
  {};
	
  class TextureCube : public TextureT<GL_TEXTURE_CUBE_MAP>
  {};

}

#endif
