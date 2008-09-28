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

#include "WatchDog.hpp"

#include <Radiant/Sleep.hpp>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace Radiant {

  WatchDog::WatchDog()
    : m_continue(false),
      m_intervalSeconds(60.0f)
  {}

  WatchDog::~WatchDog()
  {
    stop();
  }

  void WatchDog::hostIsAlive()
  {
    m_check = true; 
  }
    
  void WatchDog::childLoop()
  {
    m_continue = true;
    m_check = true; 

    while(m_continue) {
      int n = (int) ceilf(m_intervalSeconds * 10.0f);

      for(int i = 0; i < n && m_continue; i++)
	Radiant::Sleep::sleepMs(100);

      if(!m_check) {
	printf("WatchDog::mainLoop # HOST IS DEAD, ABORTING\n");
	fflush(0);
	abort();
      }

      m_check = false;
    }
  }

  void WatchDog::stop()
  {
    if(!m_continue)
      return;

    m_continue = false;
    waitEnd();
  }

}
