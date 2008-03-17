#ifndef RADIANT_STANDARD_PATHS_HPP
#define RADIANT_STANDARD_PATHS_HPP

#include <string>

namespace Radiant
{

  /// Default font search paths (no trailing /) 
  const std::string STD_FONT_PATHS("../Fonts:/usr/share/fonts/truetype/ttf-dejavu:.");

/*
  class Path
  {    
    void addPath(const std::string & path);
    const std::string & path() const;

    /// Find the given file in all registered paths
    /// @param filename name of the file
    /// @return full path to the file, empty if not found
    std::string findFile(const std::string & filename) const;
  };
*/
 
}

#endif
