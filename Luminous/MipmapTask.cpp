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
#include "MipmapTask.hpp"

#include <Luminous/Image.hpp>

#include <Radiant/Directory.hpp>
#include <Radiant/Trace.hpp>
#include <Radiant/FileUtils.hpp>

#include <Magick++.h>

#include <sstream>
#include <fstream>

namespace Luminous
{

  MipmapTask::MipmapTask(const std::string & filename, ImagePyramid * pyramid, int maxDimExp, int desiredLevels)
    : m_filename(filename),
    m_pyramid(pyramid),
    m_maxDimExp(maxDimExp),
    m_maxLevels(desiredLevels),
    m_origSize(0, 0),
    m_origImage(0)
  {
  }

  MipmapTask::~MipmapTask()
  {
    delete m_origImage;
  }


  std::string MipmapTask::getLevelCacheFilename(int level) const
  {
    /* Put the cache files into separate directory so they are easier
       to delete and ignore. */

    std::string cachedir = Radiant::FileUtils::path(m_filename) + "/.cache";

    Radiant::Directory::mkdir(cachedir);
    
    std::ostringstream os;
    os << Radiant::FileUtils::path(m_filename) << 
      "/.cache/" << 
      level << 
      "_" <<
      Radiant::FileUtils::filename(m_filename);

    return os.str();
  }

  void MipmapTask::initialize()
  {
    Radiant::trace("MipmapTask::initialize # %s", m_filename.c_str());

    m_origImage = new Magick::Image();

    // Just read the original image size
    try {
      m_origImage->read(m_filename);
      m_origSize = Nimble::Vector2i((int)m_origImage->columns(), (int)m_origImage->rows());
    } catch(...) {
      Radiant::error("MipmapTask::initialize # failed to read '%s'", m_filename.c_str());
      m_state = Task::DONE;
      return;
    }
  }

  void MipmapTask::doTask()
  {
    Radiant::trace("MipmapTask::doTask #");

    /// @todo rewrite

/* 
    if(first) readImageSize()

    int levelToLoad = loadedLevels();

    img = readLevelFromCache()

    if(!img) {
      img = readOriginalImage() 
      img.scale()
      writeLevelToCache()
    }
  
    storeLevel()

    if(loadingComplete()) {
      state = DONE;
    }
*/

    m_state = Task::RUNNING;

    int level = loadedLevels();
    std::string levelCache = getLevelCacheFilename(level);

    Magick::Image next;

    try {
      next.read(levelCache);
    } catch(...) {
      Radiant::trace("MipmapTask::doTask # failed to load level %d from cache", level);

      // Loading the cached mipmap failed; see if we need the mipmap by
      // comparing the dimension to the original image

      // Check if we reached the limit
      int maxDim = std::max(m_origImage->columns(), m_origImage->rows());
      int levelDim = mipmapLevelDim(level);

      // Unless we have nothing loaded, don't upscale
      if(level > 0 && maxDim < levelDim) {
        Radiant::trace("MipmapTask::doTask # Finished loading\n\t\
            Reason: do not upscale\n\t\
            Texture: %s\n\t\
            Level: %d\n", m_filename.c_str(), level);
        m_state = Task::DONE;
        return;
      }

      // Scale from the original image
      next = *m_origImage;

      Magick::Geometry newSize(levelDim, levelDim);
      newSize.aspect(false);

      next.scale(newSize);

      // Save
      try {
        next.write(levelCache);
      } catch(...) {
        Radiant::error("MipmapTask::doTask # failed to write cached mipmap to disk");
        m_state = Task::DONE;
      }
    }

    // Convert to Luminous::Image
    Magick::Blob blob;
    next.magick("RGBA");
    next.write(&blob);
    
    Luminous::Image * image = new Luminous::Image();
    image->fromData((unsigned char *)blob.data(), next.columns(), next.rows(), PixelFormat(PixelFormat::LAYOUT_RGBA, PixelFormat::TYPE_UBYTE));

    m_pyramid->addLevel(image);
    
    Radiant::trace("DEBUG: ADDED NEW MIPMAP %d,%d (%d total)", image->width(), image->height(), loadedLevels());

    if(loadedLevels() == m_maxLevels) {
      Radiant::trace("MipmapTask::load # finished loading (desired mipmaps reached)");
      m_state = Task::DONE;
    }
  }

  int MipmapTask::loadedLevels() const
  {
    return m_pyramid->levels();
  }

  int MipmapTask::mipmapLevelDim(int level) const
  {
    int m = (m_maxDimExp - (m_maxLevels - 1)) + level;
    return (1 << m);
  }

  Nimble::Vector2 MipmapTask::origSize() const
  {
    return m_origSize;
  }

}
