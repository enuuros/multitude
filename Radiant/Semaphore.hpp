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

#ifndef RADIANT_SEMAPHORE_HPP
#define RADIANT_SEMAPHORE_HPP

#include <Patterns/NotCopyable.hpp>

#include <semaphore.h>

namespace Radiant {

  /// A generalized mutex with internal counter
  /** Semaphores are mostly used to limits the access to some resource
      to N concurrent users.

      A typical example might be that you have a collection of threads
      doing task X (for example file-system access) and you want to
      limit the number of threads that perform this task at the same
      time to (for example) 5:

      
      Semaphore sem(5);

      ...

      Task::doSometing()
      {
        sem.acquire();

	doTheTaskT();

	sem.release();
      }
      
  */
  class Semaphore : public Patterns::NotCopyable
  {
  public:
    Semaphore(int n)
    { sem_init( & m_semaphore, 0, n); }
    ~Semaphore()
    { sem_destroy( & m_semaphore); }
    
    /// Acquire one counter value from the Semaphore
    inline int acquire() { return sem_wait( & m_semaphore); }
    /// Acquire multiple values form the Semaphore
    inline int acquire(int n)
    {
      for(int i = 0; i < n ; i++) {
        int e = sem_wait( & m_semaphore);
        if(e)
          return e;
      }
      return 0;
    }

    /// Release one counter value back to the semaphore
    inline int release() { return sem_post( & m_semaphore); }
    /// Release multiple counter values back to the semaphore
    inline int release(int n)
    {
      for(int i = 0; i < n ; i++) {
        int e = sem_post( & m_semaphore);
        if(e)
          return e;
      }
      return 0;
    }

  private:
    sem_t m_semaphore;
  };

}

#endif
