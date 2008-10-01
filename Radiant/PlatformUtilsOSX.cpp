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

#include "PlatformUtils.hpp"

#include "FixedStr.hpp"
#include "Trace.hpp"

#include <dlfcn.h>

#include <CoreFoundation/CoreFoundation.h>

namespace Radiant
{

  namespace PlatformUtils
  {

    std::string getExecutablePath() 
    {
      CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
      
      FixedStrT<512> buf;

      CFURLGetFileSystemRepresentation(url, true,
				       (UInt8*) buf.str(), buf.capacity());
      return buf.str();
    }

    std::string getUserHomePath()
    {
      return std::string(getenv("HOME"));  
    }

    std::string getModuleGlobalDataPath(const char * module, bool isapplication)
    {
      assert(strlen(module) < 128);
      char buf[312];

      if(isapplication) {
	sprintf(buf, "/Applications/%s.app/Contents/Resources", module);
      }
      else {
	sprintf(buf, "/Library/Frameworks/%s.framework", module);
      }
      return buf;
    }

    std::string getModuleUserDataPath(const char * module, bool isapplication)
    {
      (void) isapplication;

      assert(strlen(module) < 128);
      char buf[312];

      sprintf(buf, "%s/Library/%s", getUserHomePath().c_str(), module);

      return buf;
    }

    bool fileReadable(const char * filename)
    {
      FILE * f = fopen(filename, "r");
      if(!f)
        return false;
      fclose(f);
      return true;
    }

    void * openPlugin(const char * path)
    {
      return dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    }

  }

}
