/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Resonant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "ControlData.hpp"

#include <Nimble/Math.hpp>

#include <Radiant/Trace.hpp>

namespace Resonant {

  static bool __verbose = true;

  static void unavailable(const char * func)
  {
    if(!__verbose)
      return;
    Radiant::error("%s # Not enough data available", func);
  }

  static void badmarker(const char * func, int32_t marker)
  {
    if(!__verbose)
      return;
    char a = marker & 0xFF;
    char b = (marker >> 8) & 0xFF;
    char c = (marker >> 16) & 0xFF;
    char d = (marker >> 24) & 0xFF;
    Radiant::error("%s # bad marker %c%c%c%c", func, a, b, c, d);
  }

  inline int32_t makeMarker(int32_t a, int32_t b, int32_t c, int32_t d)
  {
    return a | (b << 8) | (c << 16) | (d << 24);
  }

  const int FLOAT_MARKER  = makeMarker(',', 'f', '\0', '\0');
  const int INT32_MARKER  = makeMarker(',', 'i', '\0', '\0');
  const int TS_MARKER     = makeMarker(',', 't', '\0', '\0');
  const int STRING_MARKER = makeMarker(',', 's', '\0', '\0');
  const int BLOB_MARKER   = makeMarker(',', 'b', '\0', '\0');

  ControlData::ControlData()
    : m_current(0)
  {}

  ControlData::~ControlData()
  {}

  void ControlData::writeFloat32(float v)
  {
    ensure(8);
    getRef<int32_t>() = FLOAT_MARKER;
    getRef<float>()   = v;
  }
  
  void ControlData::writeInt32(int32_t v)
  {
    ensure(8);
    getRef<int32_t>() = INT32_MARKER;
    getRef<int32_t>() = v;
  }
  
  void ControlData::writeTimeStamp(int64_t v)
  {
    ensure(12);
    getRef<int32_t>() = TS_MARKER;
    getRef<int64_t>() = v;
  }
    
  void ControlData::writeString(const char * s)
  {
    int len = strlen(s);
    int space = stringSpace(s);
    ensure(4 + space);

    getRef<int32_t>() = STRING_MARKER;
    char * ptr =  getPtr<char>(space);
    if(len)
      memcpy(ptr, s, len + 1);
    else
      getRef<int32_t>() = 0;
  }

  float ControlData::readFloat32(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      return 0.0f;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == FLOAT_MARKER)
      return getRef<float>();
    else if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(ok)
      *ok = false;

    skipParameter(marker);
    
    return 0.0f;
  }

  int32_t ControlData::readInt32(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      unavailable("ControlData::readInt32");
      return 0;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(marker == FLOAT_MARKER)
      return Nimble::Math::Round(getRef<float>());
    else if(ok) {
      badmarker("ControlData::readInt32", marker);
      *ok = false;
    }

    skipParameter(marker);
    return 0;
  }

  int64_t ControlData::readTimeStamp(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return 0;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == TS_MARKER)
      return getRef<int64_t>();
    else if(ok)
      *ok = false;

    skipParameter(marker);
    return 0;
  }
  
  bool ControlData::readString(char * str, int maxbytes)
  {
    int32_t marker = getRef<int32_t>();

    if(marker != STRING_MARKER) {
      skipParameter(marker);
      return false;
    }
    const char * source = & m_buf[m_current];
    int len = strlen(source);

    skipParameter(marker);

    if(len > maxbytes) {
      str[0] = 0;
      return false;
    }
    
    memcpy(str, source, len + 1);

    return true;
  }

  void ControlData::ensure(unsigned bytes)
  {
    unsigned need = m_current + bytes;
    if(need > m_buf.size()) {
      m_buf.resize(need + 128 + need / 16);
    }
    m_total = m_current + bytes;
  }

  void ControlData::skipParameter(int marker)
  {
    if(marker == INT32_MARKER ||
       marker == FLOAT_MARKER)
      m_current += 4;
    else if(marker == TS_MARKER)
      m_current += 8;
    else if(marker == STRING_MARKER) {
      const char * str = & m_buf[m_current];
      m_current += stringSpace(str);
    }
  }
  
  int ControlData::stringSpace(const char * str)
  {
    int len = strlen(str) + 1;

    int rem = len & 0x3;

    int pad = rem ? 4 - rem : 0;
    return len + pad;
  }

}
