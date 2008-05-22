#include <Radiant/ResourceLocator.hpp>

#include <Radiant/Endian.hpp>
#include <Radiant/Trace.hpp>
#include <Radiant/FileUtils.hpp>
#include <Radiant/PlatformUtils.hpp>

#include <sstream>

namespace Radiant
{

  ResourceLocator::ResourceLocator()
  {}

  ResourceLocator::~ResourceLocator()
  {}

  void ResourceLocator::addPath(const std::string & path, bool front)
  {
    if(path.size() == 0)
      ;
    else if(m_paths.empty()) 
      m_paths = path;
    else {
      std::ostringstream os;

      if(front)
	os << path << ":" << m_paths;
      else
	os << m_paths << ":" << path;

      m_paths = os.str();
    }
  }

  void ResourceLocator::addModuleDataPath(const std::string & module,
					  bool front)
  {
    addPath(PlatformUtils::getModuleGlobalDataPath(module.c_str(),
						   false), front);
#ifdef RADIANT_OSX

#elif defined(RADIANT_LINUX)

#endif
  }

  std::string ResourceLocator::locate(const std::string & file) const
  {
    std::string r = FileUtils::findFile(file, m_paths);

    trace("ResourceLocator::locate # using %s", r.c_str());

    return r;
  }

  std::string ResourceLocator::locateOverWriteable(const std::string & file)
    const
  {
    return FileUtils::findOverWritable(file, m_paths);
  }

}
