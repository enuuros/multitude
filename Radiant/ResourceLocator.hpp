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

    /// Add a path to the list to search though
    /** @param front If front is true, then the new path is put to the
	front of the list, otherwise it is put at the end of the
	list.  */
    void addPath(const std::string & path, bool front = false);
    void addModuleDataPath(const std::string & module,
			   bool front = false);

      /// Locate a file
      std::string locate(const std::string & file) const;
      std::string locateOverWriteable(const std::string & file) const;

    private:
      std::string m_paths;
  };

}

#endif
