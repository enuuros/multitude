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

#ifndef RADIANT_CONDITION_HPP
#define RADIANT_CONDITION_HPP

#include <sys/time.h>
#include <time.h>

#include <Patterns/NotCopyable.hpp>

#include <Radiant/Mutex.hpp>

namespace Radiant {

  /// Condition for threads.
  /** 
      Typical use pattern for thread that waits:

      <PRE>
      mutex.lock();

      while(needMoreData())
        condition.wait(mutex);

      mutex.unlock();
      </PRE>

      Typical use pattern for thread that informs its children:

      <PRE>
      mutex.lock();
      condition.wakeAll();
      mutex.unlock();
      </PRE>

      Or simply:

      <PRE>
      condition.wakeAll(mutex);
      </PRE>
  */
  class EXPORT Condition : public Patterns::NotCopyable
  {
  public:
    Condition() 
    { pthread_cond_init(&m_cond, 0); }
    
    ~Condition() { pthread_cond_destroy(&m_cond); }
    
    int wait(Mutex &mutex)
    { return pthread_cond_wait(& m_cond, & mutex.pthreadMutex()); }
    int wait(Mutex &mutex, int millsecs)
    { 
      struct timespec abstime;
      struct timeval tv;
      gettimeofday(&tv, 0);
      tv.tv_sec += millsecs / 1000;
      tv.tv_usec += 1000 * (millsecs % 1000);
      if(tv.tv_usec >= 1000000) {
	tv.tv_sec++;
	tv.tv_usec -= 1000000;
      }
      abstime.tv_sec = tv.tv_sec;
      abstime.tv_nsec = 1000 * tv.tv_usec;
      return pthread_cond_timedwait(& m_cond, & mutex.pthreadMutex(),
				    & abstime);
    }
    
    int wakeAll() 
    { return pthread_cond_broadcast(& m_cond); }
    
    int wakeAll(Mutex &mutex) 
    { 
      mutex.lock();
      int r = pthread_cond_broadcast(& m_cond); 
      mutex.unlock();
      return r;
    }
    
    int wakeOne() 
    { return pthread_cond_signal(& m_cond); }
    
    int wakeOne(Mutex &mutex) 
    {
      mutex.lock();
      int r = wakeOne();
      mutex.unlock();
      return r;
    }
    
  protected:
    pthread_cond_t m_cond;
  };
}

#endif
