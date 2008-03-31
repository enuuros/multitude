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

#ifndef RADIANT_PRIORITY_HPP
#define RADIANT_PRIORITY_HPP

#include <sched.h>

#include <errno.h>

namespace Radiant {

  enum Priority {
    PRIORITY_AUDIO_IO = 80,
    PRIORITY_MAX = 99
  };

  int realTimeScheduling(int pri)
  {
    sched_param sp;
    sp.sched_priority = pri;

    if(sched_setscheduler(0, SCHED_FIFO, & sp) == 0)
      return 0;
    else {
      int r = errno;
      errno = 0;
      return r;
    }
  }
}

#endif


