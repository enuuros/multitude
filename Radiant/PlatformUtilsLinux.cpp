#include "PlatformUtils.hpp"
#include "Trace.hpp"

#include <dlfcn.h>

#include <assert.h>

namespace Radiant
{

  namespace PlatformUtils
  {

    std::string getExecutablePath() 
    {
      char buf[512];
      int len;

      len = readlink("/proc/self/exe", buf, 512);

      if(len == -1) {
        error("PlatformUtils::getExecutablePath # readlink() failed");
        return std::string("");
      }

      return std::string(buf, len);
    }

    std::string getUserHomePath()
    {
      return std::string(getenv("HOME"));  
    }

    std::string getModuleGlobalDataPath(const char * module, bool isapplication)
    {
      (void) isapplication;

      assert(strlen(module) < 128);
      char buf[312];
      sprintf(buf, "/usr/share/%s", module);

      return buf;
    }

    std::string getModuleUserDataPath(const char * module, bool isapplication)
    {
      (void) isapplication;

      assert(strlen(module) < 128);
      char buf[312];

      sprintf(buf, "%s/.%s", getUserHomePath().c_str(), module);

      return buf;
    }

    void * openPlugin(const char * path)
    {
      return dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    }

  }

}
