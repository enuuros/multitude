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

#ifndef RADIANT_WATCHDOG_HPP
#define RADIANT_WATCHDOG_HPP

#include <Radiant/Thread.hpp>

namespace Radiant {

  class WatchDog : public Radiant::Thread
  {
  public:
    WatchDog();
    virtual ~WatchDog();

    void hostIsAlive();

    void setInterval(float seconds)
    { m_intervalSeconds = seconds; }

    void stop();

  private:
    
    virtual void childLoop();

    volatile bool m_check;
    volatile bool m_continue;
    float m_intervalSeconds;
  };
  
}

#endif
