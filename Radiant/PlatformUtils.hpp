/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef RADIANT_PLATFORM_UTILS_HPP
#define RADIANT_PLATFORM_UTILS_HPP

#include <Radiant/Export.hpp>

#include <string>

namespace Radiant 
{
  /** Small utility functions to help handle platform-specific functions. */

  namespace PlatformUtils
  {
    
    /// Return absolute path to the executable that was used to launch the process.
    RADIANT_API std::string getExecutablePath();

    /// Return absolute path to the user's home directory.
    RADIANT_API std::string getUserHomePath();

    /// Return path to the global data directory of the given module.
    RADIANT_API std::string getModuleGlobalDataPath(const char * module, bool isapplication);

    /// Return path to the user data directory of the given module.
    RADIANT_API std::string getModuleUserDataPath(const char * module, bool isapplication);

    /// Determine whether file or directory is readable.
    RADIANT_API bool fileReadable(const char * filename);

    /// Open a dynamic library, return 0 if failed.
    RADIANT_API void * openPlugin(const char * path);

  }

}

#endif
