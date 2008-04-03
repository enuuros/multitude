#ifndef LUMINOUS_MIPMAP_TASK_HPP
#define LUMINOUS_MIPMAP_TASK_HPP

#include <Luminous/Task.hpp>
#include <Luminous/ImagePyramid.hpp>

#include <Nimble/Vector2.hpp>


/*
  Usage goal:

  DynamicTexture tex;
  BGThread::addTask("foo.png", new MipmapTask(tex.mipmapPyramid(), 10, 1));

  <draw stuff, tex takes care of stuff under the hood>
*/

namespace Magick
{
  class Image;
}

namespace Luminous
{
  /// A task to generate (pruned) mipmap pyramids
  class MipmapTask : public Task
  {
    public:
      MipmapTask(const std::string & filename, ImagePyramid * pyramid, int maxDimExp, int desiredLevels);
      ~MipmapTask();

      virtual void doTask();

      std::string filename() const { return m_filename; }
    
      virtual void initialize();

      Nimble::Vector2 origSize() const;

      ImagePyramid * pyramid() { return m_pyramid; }

    private:
      int loadedLevels() const;
      std::string getLevelCacheFilename(int level) const;
      int mipmapLevelDim(int level) const;

      std::string m_filename;
      ImagePyramid * m_pyramid;
      int m_maxDimExp;
      int m_maxLevels;

      Nimble::Vector2i m_origSize;  
      Magick::Image * m_origImage;
  };

}

#endif