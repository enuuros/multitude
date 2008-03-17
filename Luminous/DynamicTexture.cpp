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

  TextureLoadable::TextureLoadable(const std::string& filepath,
				   int maxDimExp, int desiredMipmaps)
    : Loadable(filepath),
      m_maxDimExp(maxDimExp),
      m_desiredMipmaps(desiredMipmaps),
      m_originalSize(0, 0)
  {}

  TextureLoadable::~TextureLoadable()
  {}

  int TextureLoadable::numMipmaps() const
  {
    return m_mipmaps.size();
  }

  Magick::Image TextureLoadable::mipmap(int n)
  {
    if(n >= (int)m_mipmaps.size()) {
      error("TextureLoadable::mipmap # argument past range");
    }
    return m_mipmaps[n];
  }

  void TextureLoadable::doTask()
  {
    m_state = Task::PROCESSING;

    int level = numMipmaps();

    // First, attempt to load a cached mipmap
    std::string mipmapCache;
    getMipmapCacheFilename(level, mipmapCache);

    Magick::Image image;

    try {
      image.read(mipmapCache);
    } catch(...) {
      trace("TextureLoadable::load # Failed to load mipmap from cache");

      // Loading the cached mipmap failed; see if we need the mipmap by
      // comparing the dimension to the original image
      
      // Try to load the cached image information
      int w, h;
      if(!readInfoCache(w, h)) {
        trace("TextureLoadable::load # Failed to load cached image info. Reading from original image");

        // No info cached, need the original image
        try {
          image.read(m_filepath);
        } catch(...) {
            goto failure;
        }

        // Create the missing info cache
        Magick::Geometry geo = image.size();
        w = (int)geo.width();
        h = (int)geo.height();

        if(!writeInfoCache(w, h)) {
          error("TextureLoadable::load # Failed to write info cache");
        } else {
          trace("TextureLoadable::load # Wrote info cache");
        }       
      }

      //trace("SET ORIGINAL SIZE TO %d, %d", w, h);
      m_originalSize.x = w;
      m_originalSize.y = h;

      // Check if we reached the limit
      int maxDim = std::max(w, h);
      int levelDim = mipmapLevelDim(level);

      trace("DEBUG: LEVEL DIM %d", levelDim);

      // Unless we have nothing loaded, don't upscale
      if(level > 0 && maxDim < levelDim) {
        trace("TextureLoadable::load # Finished loading\n\t\
              Reason: do not upscale\n\t\
              Texture: %s\n\t\
              Level: %d\n\t\
              Dimensions: %dx%d", m_filepath.c_str(), level, w, h);
        m_state = Task::DONE;
        return;
      }

      // Now we need the original image to create the mipmap
      try {
        image.read(m_filepath);

				m_originalSize = Nimble::Vector2(image.columns(), image.rows());
      } catch(...) {
        goto failure;
      }
      
      // Scale
      Magick::Geometry newSize(levelDim, levelDim);
      newSize.aspect(false);
      
      image.scale(newSize);
      
      // Save
      try {
        image.write(mipmapCache);
      } catch(...) {
        error("TextureLoadable::load # Failed to write cached mipmap to disk");
      }
    }

    trace("DEBUG: ADDED NEW MIPMAP (%d total)", (int)m_mipmaps.size());
    m_mipmaps.push_back(image);

    if(numMipmaps() == m_desiredMipmaps) {
      trace("TextureLoadable::load # Finished loading (desired mipmaps reached)");
      m_state = Task::DONE;
    }

    return;

failure:
    error("TextureLoadable::load # Failed to load '%s'", m_filepath.c_str());
    m_state = Task::FAILURE;
  }

  void TextureLoadable::getInfoCacheFilename(std::string& cacheFile) const
  {
    cacheFile = FileUtils::path(m_filepath) + "cache_" + FileUtils::baseFilename(m_filepath) + ".size"; 
  }

  void TextureLoadable::getMipmapCacheFilename(int level, std::string& cacheFile) const
  {
    std::ostringstream os;
    os << FileUtils::path(m_filepath) << "cache_" << level << "_" << FileUtils::filename(m_filepath);
    
    cacheFile = os.str();
  }

  bool TextureLoadable::readInfoCache(int& w, int& h) const
  {
    ifstream file;

    std::string filename;
    getInfoCacheFilename(filename);

    trace("TextureLoadable::readInfoCache # trying %s", filename.c_str());
    file.open(filename.c_str(), ios::binary);

    if(!file.good()) return false;

    file.read((char*)&w, sizeof(float));
    file.read((char*)&h, sizeof(float));

    trace("\tGot %dx%d", w, h);

    file.close();

    return true;
  }

  bool TextureLoadable::writeInfoCache(int w, int h) const
  {
    trace("TextureLoadable::writeInfoCache #");
    ofstream file;

    std::string filename;
    getInfoCacheFilename(filename);

    file.open(filename.c_str(), ios::binary);

    if(!file.good()) return false;

    file.write((char*)&w, sizeof(float));
    file.write((char*)&h, sizeof(float));

    trace("\tWrote %dx%d", w, h);
  
    file.close();

    return true;
  }
  
  int TextureLoadable::mipmapLevelDim(int level) const
  {
    int m = (m_maxDimExp - (m_desiredMipmaps - 1)) + level;

    return (1 << m);
  }

  ////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  DynamicTexture::DynamicTexture(TextureLoadable* tl, GLResources * resources)
    : GLResource(resources),
      m_loadable(tl),
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
      int dim = m_loadable->mipmapLevelDim(level);

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
    return (m_loadable ? m_loadable->numMipmaps() : 0);
  }

  Texture2D* DynamicTexture::getMipmap(int n)
  {
    if(n >= (int)m_mipmaps.size()) 
      return 0;

    return m_mipmaps[n].ptr();
  }

  void DynamicTexture::updateGPUMipmaps()
  {
//    trace("DynamicTexture::updateGPUMipmaps # %s", m_loadable->filename().c_str());

    // Nothing we can do if nothing has been loaded yet
    if(m_loadable->numMipmaps() == 0) {
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
    int residentDim = (residentLevel >= 0 ? m_loadable->mipmapLevelDim(residentLevel) : 0);
    int nextLowerDim = (residentLevel > 0 ? m_loadable->mipmapLevelDim(residentLevel - 1) : 0);

    int desiredDim = Nimble::Math::Max(m_desiredSize.x, m_desiredSize.y);

    if(desiredDim > residentDim) {
      // If there's no more data, nothing we can do
      if(mipmapsOnCPU() <= mipmapsOnGPU()) {
        return;
      }
    
      //trace("\tDesired %dx%d\n\tResident %d", m_desiredSize.x, m_desiredSize.y, residentDim);
      //trace("\tMipmaps on CPU %d, on GPU %d", mipmapsOnCPU(), mipmapsOnGPU());
      
      // Upload a sharper mipmap
      Magick::Image img = m_loadable->mipmap(residentLevel + 1);

      Texture2D * tex = Texture2D::fromImage(img, false, resources());
      if(!tex) {
        error("DynamicTexture::updateGPUMipmaps # Failed to create a texture mipmap");
        return;
      }
      
      m_mipmaps.push_back(Radiant::RefPtr<Texture2D>(tex));
      trace("\tUploaded mipmap level %d", residentLevel + 1);

//    } else if(desiredDim < residentDim) {
      } else if(desiredDim < nextLowerDim) {    

      // Anything to throw away? Should we throw the last level away?
      if(residentLevel <= 0) {
        return;
      }

      m_mipmaps.pop_back();
      trace("\tPurged mipmap level %d", residentLevel);     
    }
  }

  void DynamicTexture::hint(const Nimble::Vector2 & size)
  {
		m_desiredSize = size;
  }
}
