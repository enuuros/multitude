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


#ifndef RADIANT_TRACE_HPP
#define RADIANT_TRACE_HPP

#define FNAME static const char * fname = __FUNCTION__ 

namespace Radiant {
  
  /* Global functions for output to console/file. */

  extern EXPORT void setAppName(const char *name);

  extern EXPORT void trace(const char *msg, ...);

  extern EXPORT void error(const char *msg, ...);

  extern EXPORT void fatal(const char *msg, ...);

}

#endif
