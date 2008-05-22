#include <Radiant/ResourceLocator.hpp>
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

  std::string ResourceLocator::locate(const std::string & path) const
  {
    std::string r = FileUtils::findFile(path, m_paths);

    trace("ResourceLocator::locate # using %s", r.c_str());

    return r;
  }

}
