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

#ifndef RADIANT_MUTEX_HPP
#define RADIANT_MUTEX_HPP

#include <Patterns/NotCopyable.hpp>

#include <Radiant/Config.hpp>

#include <pthread.h>

namespace Radiant {

#ifdef RADIANT_HAVE_PTHREAD

  /** Mutex class. The mutex must be initialized explicitly. */
  class MTEXPORT Mutex : public Patterns::NotCopyable
  {
  public:
    ///
    Mutex();

    ///
    virtual ~Mutex();
  
    /** Initialize the mutex. 
   
    Setting all values to false gives usually the best performance.

    The boolean arguments refer to POSIX-functionality with the same
    name. Not all features work on all platforms how-ever: My Linux
    box does not know anything about inheriting priorities. 

    Recursion should work on all platforms. */
    bool init(bool shared = false, 
	      bool prio_inherit = true,
	      bool recursive = true);
  
    /// Close the mutex.
    bool close();

    /** Locks the mutex. Blocks until mutex is available. */
    bool lock(); 

    /// Lock the mutex, optionally blocking.
    bool lock(bool block);

    /** Tries to lock the mutex. Does not block. */
    bool tryLock();

    /// Unlocks the mutex.
    bool unlock();

    /// Self test.
    static void test();

    pthread_mutex_t & pthreadMutex() { return m_mutex; }

  protected:

    pthread_mutex_t m_mutex;
    bool            m_active;
  };

#else

#include <cstdio>

  /** Dummy implementation of a mutex. */

  class Mutex
  {
  public:
    Mutex() { puts("Mutex # Dummy mutex in use"); }
    ~Mutex() {}
    bool init(bool, bool, bool) { return true; }
    bool close() { return true; }
    bool lock() { return true; }
    bool lock(bool) { return true; }
    bool tryLock() { return true; }
    bool unlock() { return true; }
    static void test() {}
  };

#endif

  /// Mutex that initializes automatically.
  class MTEXPORT MutexAuto : public Mutex
  {
  public:
    /// Calls init.
    MutexAuto(bool shared = false, 
	      bool prio_inherit = true,
	      bool recursive = true) 
    { init(shared, prio_inherit, recursive); }
    ~MutexAuto() {}
  };


#ifdef __linux__ 

  typedef MutexAuto MutexStatic;

#else

  /// Mutex class to be used as static or global variable

  /** Under Linux, this class is simply typedef to MutexAuto. On other
      platforms (OSX, Windows) there is some trouble initializing
      mutexes as static variables as the application/library is
      loaded. For these cases there is an implementation that
      initializes when the mutex is first used. 

      This can be problematic, if the mutex is accessed from two
      threads at exactly the same time for the first time. How-over,
      the probability of getting errors in that phase are extremely
      small. */
  class MutexStatic : public Mutex
  {
  public:
    MutexStatic() {}
    
    bool lock() { if(!m_active) init(); return Mutex::lock(); }
    bool lock(bool b) { if(!m_active) init(); return Mutex::lock(b); }
    bool tryLock() { if(!m_active) init(); return Mutex::tryLock(); }
  };
#endif

  /** A guard class. This class is used to automatically lock and unlock
      a mutex within some function.

      @see ReleaseGuard
  */

  class Guard : public Patterns::NotCopyable
  {
  public:
    /// Locks the mutex
    Guard(Mutex * mutex) : m_mutex(mutex) { m_mutex->lock(); }
    
    /// Unlocks the mutex
    ~Guard() { m_mutex->unlock(); }

  private:
    Mutex * m_mutex;

  };

  /** A guard class that only releases a locked mutex. This class is
      used to automatically unlock a mutex within some function.

      @see Guard
  */

  class ReleaseGuard : public Patterns::NotCopyable
  {
  public:
    /// Locks the mutex
    ReleaseGuard(Mutex * mutex) : m_mutex(mutex) { }
    
    /// Unlocks the mutex
    ~ReleaseGuard() { m_mutex->unlock(); }

  private:
    Mutex * m_mutex;
  };

}


#endif
