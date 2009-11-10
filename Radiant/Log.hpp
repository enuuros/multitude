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

#ifndef RADIANT_LOG_HPP
#define RADIANT_LOG_HPP

#include <Radiant/Export.hpp>

namespace Radiant {

  /** Provides logging sevices for the application. The logging system
      is thread-safe, and it is meant for applications to store usage
      information.

      The log mechanism is is not meant for debug output or error
      messages. For those purposes, see Radiant::info, and
      Radiant::error.*/
  class RADIANT_API Log
  {
  public:

    /** Sets the log file. 
	
	@return This function true if the the new log file was opened
	successfully, and false if opening failed.
     */ 
    static bool setLogFile(const char * logfile);
    static bool setTimedLogFile(const char * appname);

    /** Saves a log message to the file. */
    static void log(const char * str, ...);

  private:
    Log() {}
    Log(const Log &) {}
  };

}

#endif
