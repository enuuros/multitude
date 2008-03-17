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

#ifndef LUMINOUS_DYNAMICTEXTURE_HPP
#define LUMINOUS_DYNAMICTEXTURE_HPP

#include "Loadable.hpp"
#include "Texture.hpp"
#include "GLResource.hpp"
#include <Radiant/RefPtr.hpp>
#include <Nimble/Vector2.hpp>
#include <vector>
#include <Magick++.h>

namespace Luminous
{

  class TextureLoadable : public Loadable
  {
    public:
      TextureLoadable(const std::string& filename, int maxDimExp = 10, int desiredMipmaps = 1);
      virtual ~TextureLoadable();

      virtual int numMipmaps() const;
      Magick::Image mipmap(int n);

      virtual void doTask();

      void getInfoCacheFilename(std::string& cacheFile) const;
      void getMipmapCacheFilename(int level, std::string& cacheFile)  const;

      int mipmapLevelDim(int level) const;

      bool readInfoCache(int& w, int& h) const;
      bool writeInfoCache(int w, int h) const;

			Nimble::Vector2i originalSize() const { return m_originalSize; }

    protected:
      typedef std::vector<Magick::Image> container;

      int m_maxDimExp;
      int m_desiredMipmaps;
      container m_mipmaps;		

			Nimble::Vector2i m_originalSize;
  };

  class DynamicTexture : public GLResource
  {
    public:
      DynamicTexture(TextureLoadable * tl, GLResources * resources = 0);
      virtual ~DynamicTexture();

      virtual Texture2D* selectMipmap(); 

      virtual int mipmapsOnGPU() const;
      virtual int mipmapsOnCPU() const;

      virtual void updateGPUMipmaps();

      virtual void hint(const Nimble::Vector2 & size);

      Texture2D* getMipmap(int n);

			Nimble::Vector2i originalSize() const { return m_loadable->originalSize(); }

    protected:
      std::vector<Radiant::RefPtr<Texture2D> > m_mipmaps;
      TextureLoadable * m_loadable;

      Nimble::Vector2i m_desiredSize;
  };

}

#endif
