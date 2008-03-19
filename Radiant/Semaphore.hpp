/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
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

  class Semaphore : public Patterns::NotCopyable
  {
  public:
    Semaphore(int n)
    { sem_init( & m_semaphore, 0, n); }
    ~Semaphore()
    { sem_destroy( & m_semaphore); }
    
    inline int acquire() { return sem_wait( & m_semaphore); }
    inline int acquire(int n)
    {
      for(int i = 0; i < n ; i++) {
        int e = sem_wait( & m_semaphore);
        if(e)
          return e;
      }
      return 0;
    }

    inline int release() { return sem_post( & m_semaphore); }
    inline int release(int n)
    {
      for(int i = 0; i < n ; i++) {
        int e = sem_post( & m_semaphore);
        if(e)
          return e;
      }
      return 0;
    }

  protected:
    sem_t m_semaphore;
  };

}

#endif
