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
#include "Trace.hpp"

#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/resource.h>

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
        Radiant::error("PlatformUtils::getExecutablePath # readlink() failed");
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

    uint64_t processMemoryUsage()
    {
      struct rusage r_usage;
      getrusage(RUSAGE_SELF, &r_usage);
      return r_usage.ru_idrss;
    }
  }

}
