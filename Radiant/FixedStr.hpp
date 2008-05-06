
#ifndef RADIANT_FIXED_STR_HPP
#define RADIANT_FIXED_STR_HPP

#include <stdio.h>

#include <Nimble/Vector4.hpp>

namespace Radiant {

  /** Template class fixed-capacity strings. */

  template <int N>
  class FixedStrT
  {
  public:

    FixedStrT() { m_buf[0] = '\0'; }
    FixedStrT(Nimble::Vector2 v, int digits = 1)
    { writeFloats(v.data(), 2, digits); }
    FixedStrT(Nimble::Vector3 v, int digits = 1)
    { writeFloats(v.data(), 3, digits); }
    FixedStrT(Nimble::Vector4 v, int digits = 1)
    { writeFloats(v.data(), 4, digits); }
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

    void writeFloats(float * ptr, int n, int digits)
    {
      char tmp[6];

      sprintf(tmp, "%%.%df ", digits);
      
      char *tmpptr = m_buf;
      
      for(int i = 0; i < n; i++) {
        tmpptr += sprintf(tmpptr, tmp, ptr[i]);
      }
    }
    
    const char * str() const { return m_buf; }

    operator const char * () const { return m_buf; }
    operator char * () { return m_buf; }

    int length() { return strlen(m_buf); }

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

  
}


#endif
