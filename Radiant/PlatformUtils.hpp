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

    /** Return path to the global data directory of the given
	module. */
    std::string getModuleGlobalDataPath(const char * module, bool isapplication);
    std::string getModuleUserDataPath(const char * module, bool isappliction);

    /// Open a dynamic library, return 0 if failed
    void * openPlugin(const char * path);
  }

}

#endif
