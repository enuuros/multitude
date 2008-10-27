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

#ifndef RADIANT_FIXED_STR_HPP
#define RADIANT_FIXED_STR_HPP

#include <Nimble/Matrix3.hpp>
#include <Nimble/Vector4.hpp>

#include <Radiant/Export.hpp>

#include <stdio.h>
#include <cassert>

namespace Radiant {

  /** Template class for fixed-capacity strings. */

  template <int N>
  class RADIANT_API FixedStrT
  {
  public:

    FixedStrT() { m_buf[0] = '\0'; }
    FixedStrT(float v, int digits = 1)
    { writeFloats( & v, 1, digits); }
    FixedStrT(Nimble::Vector2 v, int digits = 1)
    { writeFloats(v.data(), 2, digits); }
    FixedStrT(Nimble::Vector3 v, int digits = 1)
    { writeFloats(v.data(), 3, digits); }
    FixedStrT(Nimble::Vector4 v, int digits = 1)
    { writeFloats(v.data(), 4, digits); }

    FixedStrT(const Nimble::Matrix3 & v, int digits = 1)
    { writeFloats(v.data(), 9, digits); }

    FixedStrT(const char * str)
    { 
      if(str) {
        assert(strlen(str) < N);
        strcpy(m_buf, str);
      }
      else
        m_buf[0] = '\0';
    }
    ~FixedStrT() {}

    void writeFloats(const float * ptr, int n, int digits)
    {
      char tmp[6];

      sprintf(tmp, "%%.%df ", digits);
      
      char *tmpptr = m_buf;
      
      for(int i = 0; i < n; i++) {
        tmpptr += sprintf(tmpptr, tmp, ptr[i]);
      }
    }
    
    inline void copyn(const char * ptr, int n)
    {
      if(n+1 >= N)
	return;

      memcpy(m_buf, ptr, n);
      m_buf[n] = 0;
    }

    const char * str() const { return m_buf; }

    operator const char * () const { return m_buf; }
    operator char * () { return m_buf; }

    int length() { return strlen(m_buf); }
    static inline int capacity() { return N; }

    FixedStrT & operator = (const char * str)
    { 
      if(str) {
        assert(strlen(str) < N);
        strcpy(m_buf, str);
      }
      else
        m_buf[0] = '\0';

      return * this;
    }

  protected:
    char m_buf[N+1];
  };

#ifdef WIN32
#ifdef RADIANT_EXPORT
  // In WIN32 template classes must be instantiated to be exported
  template class FixedStrT<32>;
  template class FixedStrT<256>;
  template class FixedStrT<512>;
#endif
#endif
  
}


#endif
