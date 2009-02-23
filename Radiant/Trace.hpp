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


#ifndef RADIANT_TRACE_HPP
#define RADIANT_TRACE_HPP

#include <Radiant/Export.hpp>

#define FNAME static const char * fname = __FUNCTION__ 

namespace Radiant {
  
  enum Severity
  {
    DEBUG,
    INFO,
    WARNING,
    FAILURE,
    FATAL    
  };

  /// Display useful output.
  RADIANT_API void trace(Severity s, const char * msg, ...);
  /// Display debug output
  /** This function calls trace to do the final work and it is
      effectively the same as calling debug(...). */
  RADIANT_API void debug(const char * msg, ...);
  /// Display information output
  /** This function calls trace to do the final work and it is
      effectively the same as calling trace(INFO, ...). */
  RADIANT_API void info(const char * msg, ...);
  /// Display error output
  /** This function calls trace to do the final work and it is
      effectively the same as calling trace(FAILURE, ...). */
  RADIANT_API void error(const char * msg, ...);

  /// Display error output, with a fatal message
  /** This function calls trace to do the final work and it is
      effectively the same as calling trace(FATAL, ...). */
  RADIANT_API void fatal(const char * msg, ...);

  /// Toggle verbose output. If enabled, messages sent with the Trace function
  /// are displayed to the user. Otherwise they are ignored. @sa Trace
  RADIANT_API void enableVerboseOutput(bool enable);

}

#endif
