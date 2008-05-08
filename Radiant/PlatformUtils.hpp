#ifndef RADIANT_PLATFORM_UTILS_HPP
#define RADIANT_PLATFORM_UTILS_HPP

#include <string>

namespace Radiant 
{

  namespace PlatformUtils
  {
    /// Return absolute path to the executable that was used to launch the
    /// process
    std::string getExecutablePath();

    /// Return absolute path to the user's home directory
    std::string getUserHomePath();

    /// Open a dynamic library, return 0 if failed
    void * openPlugin(const char * path);
  }

}

#endif
