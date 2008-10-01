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

#ifndef RADIANT_SLEEP_HPP
#define RADIANT_SLEEP_HPP

#include <Radiant/Export.hpp>
#include <Radiant/Types.hpp>

#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#ifdef WIN32
#include <WinPort.h>		// for sleep() and nanosleep()
#include <pthread.h>		// for struct timespec
#endif

#define RADIANT_BILLION 1000000000
#define RADIANT_MILLION 1000000

namespace Radiant {

  class Mutex;
  class TimeStamp;

  enum Timing {
    TIMING_GOOD,
    TIMING_LATE,
    TIMING_EARLY
  };

  // POSIX-specific functions:

  inline void addTimeNs(struct timespec *tspec, long ns)
  {
    tspec->tv_nsec += ns;
    if(tspec->tv_nsec >= RADIANT_BILLION) {
      long tmp = tspec->tv_nsec / RADIANT_BILLION;
      tspec->tv_sec += tmp;
      tspec->tv_nsec = tspec->tv_nsec - tmp * RADIANT_BILLION;
    }
  }

  inline void addTimeUs(struct timeeval *tspec, long us)
  {
    tspec->tv_usec += us;
    if(tspec->tv_usec >= RADIANT_MILLION) {
      long tmp = tspec->tv_usec / RADIANT_MILLION;
      tspec->tv_sec += tmp;
      tspec->tv_usec = tspec->tv_usec - tmp * RADIANT_MILLION;
    }
  }

  inline void addTime(struct timeeval *tspec, const struct timeeval *tspecAdd)
  {
    tspec->tv_sec  += tspecAdd->tv_sec;
    tspec->tv_usec += tspecAdd->tv_usec;
    if(tspec->tv_usec >= RADIANT_MILLION) {
      long tmp = tspec->tv_usec / RADIANT_MILLION;
      tspec->tv_sec += tmp;
      tspec->tv_usec = tspec->tv_usec - tmp * RADIANT_MILLION;
    }
  }

  inline long timeDiffNs(const struct timespec *tspecOld,
			 const struct timespec *tspecNew)
  {
    return long((tspecNew->tv_sec - tspecOld->tv_sec) * RADIANT_BILLION +
      tspecNew->tv_nsec - tspecOld->tv_nsec);
  }

  inline long timeDiffUs(const struct timeeval *tspecOld,
			 const struct timeeval *tspecNew)
  {
    return long((tspecNew->tv_sec - tspecOld->tv_sec) * RADIANT_MILLION +
      tspecNew->tv_usec - tspecOld->tv_usec);
  }

  /** Sleeping services. This class contains only static member
      functions. The constructor and destructor are included to prevent
      compiler warnings.*/
  class RADIANT_API Sleep
  {
  public:

    Sleep() { }

    ~Sleep() {}

    /// Sleep for n seconds.
    static bool sleepS(uint secs) { return ! sleep(secs); }

    /** Sleep for n milliseconds. You cannot sleep more than one second
	with this function. */
    static bool sleepMs(uint msecs) 
    { 
      struct timespec tspec;
      tspec.tv_sec = 0;
      tspec.tv_nsec = (long) msecs * RADIANT_MILLION;
      return nanosleep(&tspec, 0) != -1;
    }

    /** Sleep for n microseconds. You cannot sleep more than one second
	with this function. */
    static bool sleepUs(uint usecs)
    { 
      struct timespec tspec;
      tspec.tv_sec = 0;
      tspec.tv_nsec = (long) usecs * 1000;
      return nanosleep(&tspec, 0) != -1;
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  /// Synchronized sleeping.
  /** This class can be used to time the execution of a thread. For
      example if you want a thread not to execute too often.*/
  class RADIANT_API SleepSync
  {
  public:
    /// The constructor resets the timing.
    SleepSync() { resetTiming(); }

    /// Resets the reference time to current time.
    void resetTiming() 
    { gettimeofday(& m_timing, 0); m_initial = m_timing; }
  
    /// Sleep for n microseconds
    /** This function calculates how much time has passed since the
	last sleep and sleeps to fulfill the required time period. */
    long sleepSynchroUs(long us);

    long sleepTo(const TimeStamp *stamp, Mutex *mutex = 0);

  private:  
    struct timeeval m_initial, m_timing;
  };

}

#endif
