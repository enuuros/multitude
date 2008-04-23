#include "SystemUtils.hpp"
#include "Trace.hpp"

namespace Radiant
{

  namespace SystemUtils
  {

    std::string getExecutablePath() 
    {
      char buf[512];
      int len;

      len = readlink("/proc/self/exe", buf, 512);

      if(len == -1) {
        error("SystemUtils::getExecutablePath # readlink() failed");
        return std::string("");
      }

      return std::string(buf, len);
    }

    std::string getUserHomePath()
    {
      return std::string(getenv("HOME"));  
    }

  }

}
