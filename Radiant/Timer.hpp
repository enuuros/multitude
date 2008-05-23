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

#ifndef RADIANT_TIMER_HPP
#define RADIANT_TIMER_HPP

#include <Radiant/TimeStamp.hpp>

namespace Radiant
{

  /// Timer provides a simple method of keeping track of elapsed time.
  class MTEXPORT Timer
  {
    public:
      Timer();

      /// Start the timer. Marks the current time as zero time.      
      void start();
      /// Return the number of seconds since the last call to start().
      float elapsed() const;

    protected:
      TimeStamp m_started;
  };
}

#endif
