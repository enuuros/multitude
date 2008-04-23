#include <Radiant/ResourceLocator.hpp>
#include <Radiant/Trace.hpp>
#include <Radiant/FileUtils.hpp>
#include <Radiant/SystemUtils.hpp>

#include <sstream>

namespace Radiant
{

  ResourceLocator::ResourceLocator()
  {}

  ResourceLocator::~ResourceLocator()
  {}

  void ResourceLocator::addPath(const std::string & path)
  {
    if(m_paths.empty()) 
      m_paths = path;
    else {
      std::ostringstream os;
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
