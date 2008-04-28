/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
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
  
  static Radiant::MutexAuto __mutex;

  static char fatalmsg[64] = "[FATAL] ";
  static char errmsg[64] = "[ERROR] ";
  static char tracemsg[64] = "[trace] ";

  static void __output(const char *str, bool isError, bool isFatal = false)
  {
    __mutex.lock();
    
    if(isFatal)
      fprintf(stdout, fatalmsg);
    else if(isError)
      fprintf(stdout, errmsg);
    else
      fprintf(stdout, tracemsg);

    fprintf(stdout, str);
    fprintf(stdout, "\n");
    fflush(stdout);

    __mutex.unlock();
  }

  void setAppName(const char *name)
  {
    __mutex.lock();
    assert(strlen(name) < 40);
    sprintf(fatalmsg, "[%s FATAL] ", name);
    sprintf(errmsg, "[%s ERROR] ", name);
    sprintf(tracemsg, "[%s] ", name);
    __mutex.unlock();
  }

  /// Print trace information to the user
  void trace(const char *msg, ...)
  {
    
    char buf[8196];
    va_list args;
    va_start(args, msg);
    vsprintf(buf, msg, args);
    
    __output(buf, false);
    va_end(args);
  }

  /// Print error information to the user
  void error(const char *msg, ...)
  {
    
    char buf[8196];
    va_list args;
    va_start(args, msg);
    vsprintf(buf, msg, args);
    
    __output(buf, true);
    va_end(args);
  }

  /// Print error information to the user
  void fatal(const char *msg, ...)
  {
    
    char buf[8196];
    va_list args;
    va_start(args, msg);
    vsprintf(buf, msg, args);
    
    __output(buf, true, true);
    va_end(args);

    int * fail = 0;
    assert(0);
    *fail = 123456;
    exit(1);
  }



}
