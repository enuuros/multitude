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

#if defined(WIN32)
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
#else
# include <sys/time.h>
#endif

/// @todo change to use Radiant::TimeStamp internally
namespace Radiant
{
  class Timer
  {
    public:
      Timer();

      void start();
      float elapsed() const;

    protected:
#if defined(WIN32)
      LARGE_INTEGER performanceFrequency;
      double performanceReciprocal;

      LARGE_INTEGER startTime;
#else
      struct timeval startTime;
#endif
  };
}

#endif
