#ifndef RADIANT_SYSTEM_UTILS_HPP
#define RADIANT_SYSTEM_UTILS_HPP

#include <string>

namespace Radiant 
{

  namespace SystemUtils
  {
    /// Return absolute path to the executable that was used to launch the
    /// process
    std::string getExecutablePath();

    /// Return absolute path to the user's home directory
    std::string getUserHomePath();
  }

}

#endif
