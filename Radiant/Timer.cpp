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

#include <Radiant/Timer.hpp>

namespace Radiant
{
#if defined(WIN32)
  Timer::Timer()
  {
    QueryPerformanceFrequency(&performanceFrequency);
    performanceReciprocal = 1.0f / (double)performanceFrequency.QuadPart;

    start();
  }

  void Timer::start()
  {
    QueryPerformanceCounter(&startTime);
  }

  float Timer::elapsed() const 
  {
    LARGE_INTEGER endTime;

    QueryPerformanceCounter(&endTime);
    float t = (float)((double)(endTime.QuadPart - startTime.QuadPart) * 
		      performanceReciprocal);
    return t;
  }
#else
  Timer::Timer() 
  {
    start();
  }

  void Timer::start()
  {
    gettimeofday(&startTime, 0);
  }

  float Timer::elapsed() const 
  {
    struct timeval endTime;

    gettimeofday(&endTime, 0);
    float t =  ((endTime.tv_sec + endTime.tv_usec * 1.0e-6)
        - (startTime.tv_sec + startTime.tv_usec * 1.0e-6));
    return t;
  }
#endif

}

