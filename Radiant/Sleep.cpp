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

#include "Sleep.hpp"

#ifdef WIN32
#include "GetTimeOfDay.hpp"
#endif

#include "Mutex.hpp"
#include "TimeStamp.hpp"



#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace Radiant {

    bool Sleep::sleepMs(uint32_t msecs)
    {
#ifdef WIN32
        ::Sleep(msecs);
        return true;
#else
      struct timespec tspec;
      tspec.tv_sec = 0;
      tspec.tv_nsec = (long) msecs * RADIANT_MILLION;
      return nanosleep(&tspec, 0) != -1;
#endif
    }

    bool Sleep::sleepUs(uint32_t usecs)
    {
#ifdef WIN32
        ::Sleep(usecs / 1000);
        return true;
#else
      struct timespec tspec;
      tspec.tv_sec = 0;
      tspec.tv_nsec = (long) usecs * 1000;
      return nanosleep(&tspec, 0) != -1;
#endif
    }


  /** Sleep until time indicated in the stamp object is passed.  The
      initialization time of this object is added to the stamp, before
      calculating sleeping period.

      The second argument should contain a locked mutex. The mutex
      will be freed while the (potential) sleeping is taking place and
      locked again.  */
  long SleepSync::sleepTo(const TimeStamp *stamp, Mutex *mutex)
  {
    struct timeval tmp;

    gettimeofday(&tmp, 0);
    m_timing.tv_sec = (long) stamp->seconds();
    m_timing.tv_usec = (long) stamp->subSecondsUS();

    addTime(& m_timing, & m_initial);

    long diffUs = timeDiffUs(&tmp, &m_timing);
    if(diffUs < 0) return diffUs;

    if(mutex) mutex->unlock();
    Sleep::sleepUs(diffUs);
    if(mutex) mutex->lock();

    gettimeofday(&tmp, 0);
    return timeDiffUs(&tmp, &m_timing);
  }

  /** Sleep in synchronous mode. The argument value is added to
      current time value. The return value tells how many microseconds
      early or late the function call returns. Zero is optimal,
      negative values indicate the call returned early and positive
      values indicate the call returned too late. */
  long SleepSync::sleepSynchroUs(long us)
  {
    struct timeval tmp;

    addTimeUs(&m_timing, us);
    gettimeofday(&tmp, 0);

    long diffUs = timeDiffUs(&tmp, &m_timing);
    if(diffUs < 0) return diffUs;

    Sleep::sleepUs(diffUs);
    gettimeofday(&tmp, 0);
    return timeDiffUs(&tmp, &m_timing);
  }

}
