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
    ERROR,
    FATAL    
  };

  /// Display useful output.
  RADIANT_API void trace(Severity s, const char * msg, ...);

  /// Toggle verbose output. If enabled, messages sent with the Trace function
  /// are displayed to the user. Otherwise they are ignored. @sa Trace
  extern RADIANT_API void enableVerboseOutput(bool enable);

}

#endif
