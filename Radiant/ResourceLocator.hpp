#ifndef RADIANT_RESOURCE_LOCATOR_HPP
#define RADIANT_RESOURCE_LOCATOR_HPP

#include <string>

namespace Radiant
{

  /// Class for locating resources. Basically it searches for a given filename
  /// through a set of paths and returns the first path that contains the given
  /// file.
  class ResourceLocator 
  {
    public:
      ResourceLocator();
      ~ResourceLocator();

      // Add a path to the list to search though
      void addPath(const std::string & path);

      /// Locate a file
      std::string locate(const std::string & file) const;

    private:
      std::string m_paths;
  };

}

#endif
