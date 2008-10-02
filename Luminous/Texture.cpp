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

#include "Texture.hpp"
#include "PixelFormat.hpp"
#include <cassert>
#include <iostream>

#include <Radiant/Trace.hpp>

using namespace std;

namespace Luminous
{
#ifndef WIN32
  template <GLenum TextureType>
  TextureT<TextureType>::~TextureT()
#else
  TextureT::~TextureT()
#endif
  {
    if(m_textureId) glDeleteTextures(1, &m_textureId);
    changeByteConsumption(consumesBytes(), 0);
  }

  template class TextureT<GL_TEXTURE_1D>;
  template class TextureT<GL_TEXTURE_2D>;
  template class TextureT<GL_TEXTURE_3D>;

  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  Texture1D* Texture1D::fromImage
  (Magick::Image & image, bool buildMipmaps, GLResources * resources)
  {
    Radiant::trace("DEBUG: img size %d,%d", image.columns(), image.rows());
    assert(image.columns() == 1);
  
    Magick::Blob blob;

    image.magick("RGBA");
    image.write(&blob);

    return fromBytes(GL_RGBA, image.rows(), blob.data(),
                     PixelFormat(PixelFormat::LAYOUT_RGBA,
                                 PixelFormat::TYPE_UBYTE) ,
                     buildMipmaps, resources);
  }

  Texture1D* Texture1D::fromBytes(GLenum internalFormat, int h,
				  const void* data,
				  const PixelFormat& srcFormat,
				  bool buildMipmaps, GLResources * resources)
  {
    // Check dimensions
    if(!GL_ARB_texture_non_power_of_two) {
      bool isPowerOfTwo = !((h - 1) & h);
      if(!isPowerOfTwo) {
        cerr << "ERROR: non-power-of-two textures are not supported" << endl;
        return 0;
      }
    }

    Texture1D* tex = new Texture1D(resources);

    tex->m_haveMipmaps = buildMipmaps;
    tex->m_width = 1;
    tex->m_height = h;
    /// @todo this is actually wrong, should convert from internalFormat really
    tex->m_pf = srcFormat;

    tex->bind();

    // Default to bilinear filtering
    GLint minFilter = GL_LINEAR;
    GLint magFilter = GL_LINEAR;

    // ...or trilinear if we have mipmaps
    if(buildMipmaps) minFilter = GL_LINEAR_MIPMAP_LINEAR;

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, magFilter);

    glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, h, 0,
                 srcFormat.layout(), srcFormat.type(), data);

    if(buildMipmaps) {
      gluBuild1DMipmaps(GL_TEXTURE_1D,
			srcFormat.numChannels(), h, 
			srcFormat.layout(), srcFormat.type(), data);
    }

    return tex;
  }

  bool Texture2D::loadImage(const char * filename, bool buildMipmaps)
  {
    try {
      Magick::Image im;
      
      im.read(filename);
      if(im.columns()) {
        loadImage(im, buildMipmaps);
	return true;
      }
    }
    catch(...) {
      
    }
    return false;
  }

  bool Texture2D::loadImage(Luminous::Image & image, bool buildMipmaps)
  {
    return loadBytes(image.pixelFormat().layout(),
		     image.width(), image.height(),
		     image.bytes(),
		     image.pixelFormat(), buildMipmaps);
  }

  bool Texture2D::loadImage(Magick::Image & image, bool buildMipmaps)
  {
    Magick::Blob blob;

    GLenum type;

    if(image.matte()) {
      image.magick("RGBA");
      type = GL_RGBA;
    }
    else {
      image.magick("RGB");
      type = GL_RGB;
    }

    image.write(&blob);

    return loadBytes(GL_RGBA, image.columns(), image.rows(), blob.data(),
		     PixelFormat(PixelFormat::LAYOUT_RGBA,
				 PixelFormat::TYPE_UBYTE), buildMipmaps);
  }

  void Texture2D::loadSubImage(Magick::Image & subImage, int x, int y)
  {
    Magick::Blob  blob;
    subImage.write(& blob);

    loadSubBytes(x, y, subImage.columns(), subImage.rows(), blob.data());
  }

  bool Texture2D::loadBytes(GLenum internalFormat, int w, int h,
			    const void * data, 
			    const PixelFormat& srcFormat,
			    bool buildMipmaps)
 {
    // Check dimensions
    if(!GL_ARB_texture_non_power_of_two) {
      bool isPowerOfTwo1 = !((w - 1) & w);
      bool isPowerOfTwo2 = !((h - 1) & h);

      if(!(isPowerOfTwo1 && isPowerOfTwo2)) {
        cerr << "ERROR: non-power-of-two textures are not supported" << endl;
        return false;
      }
    }

    long used = consumesBytes();

    m_width = w;
    m_height = h;
    m_pf = srcFormat;
    m_haveMipmaps = buildMipmaps;

    bind();

    // Default to bilinear filtering
    GLint minFilter = GL_LINEAR;
    GLint magFilter = GL_LINEAR;

    // ...or trilinear if we have mipmaps
    if(buildMipmaps) minFilter = GL_LINEAR_MIPMAP_LINEAR;

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    if(buildMipmaps)
      gluBuild2DMipmaps(GL_TEXTURE_2D, srcFormat.numChannels(),
			w, h, srcFormat.layout(), srcFormat.type(), data);
    else
      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0,
                   srcFormat.layout(), srcFormat.type(), data);

    long uses = consumesBytes();

    changeByteConsumption(used, uses); 

    return true;
  }

  void Texture2D::loadSubBytes(int x, int y, int w, int h, const void * data)
  {
    if(m_haveMipmaps)
    {
      // @todo
    }
    else
    {
      glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, m_pf.layout(), m_pf.type(), data);
    }
  }

  Texture2D* Texture2D::fromImage
  (Luminous::Image & image, bool buildMipmaps, GLResources * resources)
  {
    return fromBytes(GL_RGBA, image.width(), image.height(), image.bytes(), image.pixelFormat(),
                     buildMipmaps, resources);
  }

  Texture2D* Texture2D::fromImage
  (Magick::Image & image, bool buildMipmaps, GLResources * resources)
  {
    Magick::Blob blob;

    image.magick("RGBA");
    image.write(&blob);

    return fromBytes(GL_RGBA, image.columns(), image.rows(), blob.data(),
		     PixelFormat(PixelFormat::LAYOUT_RGBA,
				 PixelFormat::TYPE_UBYTE),
                     buildMipmaps, resources);
  }

  Texture2D* Texture2D::fromBytes(GLenum internalFormat, int w, int h,
				  const void* data,
				  const PixelFormat& srcFormat,
				  bool buildMipmaps, GLResources * resources)
  {
    // Check dimensions
    if(!GL_ARB_texture_non_power_of_two) {
      bool isPowerOfTwo1 = !((w - 1) & w);
      bool isPowerOfTwo2 = !((h - 1) & h);

      if(!(isPowerOfTwo1 && isPowerOfTwo2)) {
        cerr << "ERROR: non-power-of-two textures are not supported" << endl;
        return 0;
      }
    }

    Texture2D* tex = new Texture2D(resources);
    if(!tex->loadBytes(internalFormat, w, h, data, srcFormat, buildMipmaps)) {
      delete tex;
      return 0;
    }
    return tex;
  }
  
}

