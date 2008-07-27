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

#include "DynamicTexture.hpp"

#include <Radiant/FileUtils.hpp>
#include <Radiant/Trace.hpp>

#include <Nimble/Math.hpp>

#include <sstream>

using namespace Radiant;
using namespace std;

namespace Luminous
{

  DynamicTexture::DynamicTexture(GLResources * resources)
    : GLResource(resources),
      m_pyramid(0),
      m_desiredSize(0, 0)
  {}

  DynamicTexture::~DynamicTexture()
  {}

  Texture2D* DynamicTexture::selectMipmap() 
  {
    // Select a mipmap based on the size hint
    int desiredMax = Nimble::Math::Max(m_desiredSize.x, m_desiredSize.y);

    int available = mipmapsOnGPU();

    if(available == 0) return 0;

    int level = 0;
    for(; level < available - 1; level++) {
      Luminous::Image * img = m_pyramid->getLevel(level).image;

      int dim = std::max(img->width(), img->height());

      if(desiredMax < dim) break;
    }

    return m_mipmaps[level].ptr();
  }

  int DynamicTexture::mipmapsOnGPU() const
  {
    return m_mipmaps.size();
  }

  int DynamicTexture::mipmapsOnCPU() const
  {
    return m_pyramid ? m_pyramid->levels() : 0;
  }

  Texture2D * DynamicTexture::getMipmap(int n)
  {
    if(n >= (int)m_mipmaps.size()) 
      return 0;

    return m_mipmaps[n].ptr();
  }

  void DynamicTexture::updateGPUMipmaps()
  {
    // Nothing we can do if nothing has been loaded yet
    if(mipmapsOnCPU() == 0) {
      return;
    }

#if 0
    // The coarsest mipmap should always be defined (or should it?)
    if(m_mipmaps.size() == 0) {
      const Image* img = m_loadable->mipmap(0).ptr();

      Texture2D* tex = Texture2D::fromImage(*img, false);
      if(!tex) {
        error("DynamicTexture::updateGPUMipmaps # Failed to create a texture mipmap");
        return;
      }
      m_mipmaps.push_back(Radiant::RefPtr<Texture2D>(tex));

      trace("DynamicTexture::updateGPUMipmaps # Uploaded mipmap 0");
    }
#endif

    // Based on the currently most detailed resident mipmap and the hint, upload
    // a new mipmap or purge the existing one

    int residentLevel = mipmapsOnGPU() - 1;
//    int residentDim = (residentLevel >= 0 ? m_loadable->mipmapLevelDim(residentLevel) : 0);
    int residentDim = (residentLevel >= 0 ? m_pyramid->levelSize(residentLevel).maximum() : 0);
//    int nextLowerDim = (residentLevel > 0 ? m_loadable->mipmapLevelDim(residentLevel - 1) : 0);
    int nextLowerDim = (residentLevel > 0 ? m_pyramid->levelSize(residentLevel - 1).maximum() : 0);

    int desiredDim = Nimble::Math::Max(m_desiredSize.x, m_desiredSize.y);

    if(desiredDim > residentDim) {
      // If there's no more data, nothing we can do
      if(mipmapsOnCPU() <= mipmapsOnGPU()) {
        return;
      }
    
      //trace("\tDesired %dx%d\n\tResident %d", m_desiredSize.x, m_desiredSize.y, residentDim);
      //trace("\tMipmaps on CPU %d, on GPU %d", mipmapsOnCPU(), mipmapsOnGPU());
      
      // Upload a sharper mipmap
//      Magick::Image img = m_loadable->mipmap(residentLevel + 1);
      Luminous::Image * img = m_pyramid->getLevel(residentLevel + 1).image;

      Texture2D * tex = Texture2D::fromImage(*img, false, resources());

      
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


      if(!tex) {
        error("DynamicTexture::updateGPUMipmaps # Failed to create a texture mipmap");
        return;
      }
      
      m_mipmaps.push_back(Radiant::RefPtr<Texture2D>(tex));
//      trace("\tUploaded mipmap level %d", residentLevel + 1);

//    } else if(desiredDim < residentDim) {
      } else if(desiredDim < nextLowerDim) {    

      // Anything to throw away? Should we throw the last level away?
      if(residentLevel <= 0) {
        return;
      }

      m_mipmaps.pop_back();
//      trace("\tPurged mipmap level %d", residentLevel);     
    }
  }

  void DynamicTexture::hint(const Nimble::Vector2 & size)
  {
		m_desiredSize = size;
  }

  float DynamicTexture::aspect(int level) const
  {
    return m_pyramid->getLevel(level).image->aspect();
  }

}
