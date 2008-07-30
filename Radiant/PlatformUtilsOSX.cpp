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

    void * openPlugin(const char * path)
    {
      return dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    }

  }

}
