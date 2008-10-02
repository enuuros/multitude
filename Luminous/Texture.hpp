/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
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

#include <Nimble/Vector2.hpp>

#include <Patterns/NotCopyable.hpp>

// JJK
// template won't generate code until it's intantiated, so at least in
// Windows you cannot separate implementation in a cpp file. So, WIN32
// version uses a non-template version of Texture classes. Looks a bit
// messy, though.

namespace Luminous
{
  class PixelFormat;

  /// Base class for different textures
#ifndef WIN32
  template<GLenum TextureType>
#endif
  class LUMINOUS_API TextureT : public GLResource, public Patterns::NotCopyable
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
    virtual ~TextureT();

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
 
    Nimble::Vector2i size() const
    { return Nimble::Vector2i(m_width, m_height); }

    void setWidth(int w) { m_width = w; }
    void setHeight(int h) { m_height = h; }

    /** Returns the aspect ratio of this texture. This operation makes
      sense mostly for 2D tetures.  */
    float aspectRatio() 
    { return m_height ? m_width / (float) m_height : 1.0f; }

    virtual long consumesBytes() 
    { 
      float used = float(m_width) * m_height * m_pf.bytesPerPixel();
      // Mipmaps used 33% more memory
      used *= (m_haveMipmaps ? (4.f / 3.f) : 1.f);
      return (long)used;
    }

    /** Returns the OpenGL texture id. */
    GLuint id() const { return m_textureId; };

    bool isDefined() const { return id() != 0; }

    const PixelFormat & pixelFormat() const { return m_pf; }

  protected:
    GLuint m_textureId;
    int m_width;
    int m_height;
    PixelFormat m_pf;
    bool m_haveMipmaps;
#ifdef WIN32
	int TextureType;
#endif
  };
  
  /// A 1D texture
#ifndef WIN32
  class Texture1D : public TextureT<GL_TEXTURE_1D>
  {
  public:
    Texture1D(GLResources * resources = 0) : TextureT<GL_TEXTURE_1D> (resources) {}
#else
  class LUMINOUS_API Texture1D : public TextureT
  {
  public:
	  Texture1D(GLResources * resources = 0) : TextureT(resources) { TextureType = GL_TEXTURE_1D; }
#endif

    static Texture1D* fromImage(Magick::Image & image, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture1D* fromBytes(GLenum internalFormat, int w, const void* data, const PixelFormat& srcFormat, bool buildMipmaps = true, GLResources * resources = 0);
  };

  /// A 2D texture
#ifndef WIN32
  class Texture2D : public TextureT<GL_TEXTURE_2D>
  {
  public:
    Texture2D(GLResources * resources = 0) : TextureT<GL_TEXTURE_2D>(resources) {}
#else
  class LUMINOUS_API Texture2D : public TextureT
  {
  public:
    Texture2D(GLResources * resources = 0) : TextureT(resources) { TextureType = GL_TEXTURE_2D; }
#endif

    bool loadImage(const char * filename, bool buildMipmaps = true);
    bool loadImage(Luminous::Image & image, bool buildMipmaps = true);
    bool loadImage(Magick::Image & image, bool buildMipmaps = true);
    void loadSubImage(Magick::Image & subImage, int x, int y);
    bool loadBytes(GLenum internalFormat, int w, int h,
		   const void* data, 
		   const PixelFormat& srcFormat,
		   bool buildMipmaps = true);
    void loadSubBytes(int x, int y, int w, int h, const void * subData);
    static Texture2D * fromImage(Luminous::Image & img, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture2D * fromImage(Magick::Image& image, bool buildMipmaps = true, GLResources * resources = 0);
    static Texture2D * fromBytes(GLenum internalFormat, int w, int h,
				const void * data, 
				const PixelFormat& srcFormat,
				bool buildMipmaps = true, GLResources * resources = 0);
  };

#ifndef WIN32
  /// A 3D texture
  class Texture3D : public TextureT<GL_TEXTURE_3D>
  {};
	
  /// A cubemap texture
  class TextureCube : public TextureT<GL_TEXTURE_CUBE_MAP>
  {};

#else
  /// A 3D texture
  class LUMINOUS_API Texture3D : public TextureT
  {
  public:
	  Texture3D(GLResources * resources = 0) : TextureT(resources) { TextureType = GL_TEXTURE_3D; }
  };
	
  /// A cubemap texture
  class LUMINOUS_API TextureCube : public TextureT
  {
  public:
	  TextureCube(GLResources * resources = 0) : TextureT(resources) { TextureType = GL_TEXTURE_CUBE_MAP; }
  };
#endif

}

#endif
