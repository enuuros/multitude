#include "PlatformUtils.hpp"
#include "Trace.hpp"

#include <dlfcn.h>

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

    void * openPlugin(const char * path)
    {
      return dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    }

  }

}
