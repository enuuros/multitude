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

#include <Radiant/Trace.hpp>

#include <Radiant/Mutex.hpp>

#include <cassert>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>

namespace Radiant {

  static Radiant::MutexAuto g_mutex;

  static bool g_enableVerboseOutput = false;

  const char * prefixes[] = {
    "[DEBUG] ",
    "",
    "[WARNING] ",
    "[ERROR] ",
    "[FATAL] "
  };

  void enableVerboseOutput(bool enable) 
  {
    g_enableVerboseOutput = enable;
  }

  void trace(Severity s, const char * msg, ...)
  {
    if(g_enableVerboseOutput || s > DEBUG) {

      FILE * out = (s > WARNING) ? stdout : stderr;

      g_mutex.lock();

      char buf[4096];
      va_list args;

      va_start(args, msg);
      vsnprintf(buf, 4096, msg, args);
      va_end(args);

      fprintf(out, "%s%s\n", prefixes[s], buf);
      fflush(out);

      g_mutex.unlock();
    }
  }

}
