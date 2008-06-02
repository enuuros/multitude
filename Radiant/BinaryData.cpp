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

#include "BinaryData.hpp"

#include <Radiant/BinaryStream.hpp>

#include <Nimble/Math.hpp>

#include <Radiant/Trace.hpp>

namespace Radiant {

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
  const int VECTOR2F_MARKER  = makeMarker(',', 'f', '2', '\0');
  const int INT32_MARKER  = makeMarker(',', 'i', '\0', '\0');
  const int INT64_MARKER  = makeMarker(',', 'l', '\0', '\0');
  const int TS_MARKER     = makeMarker(',', 't', '\0', '\0');
  const int STRING_MARKER = makeMarker(',', 's', '\0', '\0');
  const int BLOB_MARKER   = makeMarker(',', 'b', '\0', '\0');

  BinaryData::BinaryData()
    : m_current(0)
  {}

  BinaryData::~BinaryData()
  {}

  void BinaryData::writeFloat32(float v)
  {
    ensure(8);
    getRef<int32_t>() = FLOAT_MARKER;
    getRef<float>()   = v;
  }
  
  void BinaryData::writeInt32(int32_t v)
  {
    ensure(8);
    getRef<int32_t>() = INT32_MARKER;
    getRef<int32_t>() = v;
  }

  void BinaryData::writeInt64(int64_t v)
  {
    ensure(12);
    getRef<int64_t>() = INT64_MARKER;
    getRef<int64_t>() = v;
  }
  
  void BinaryData::writeTimeStamp(int64_t v)
  {
    ensure(12);
    getRef<int32_t>() = TS_MARKER;
    getRef<int64_t>() = v;
  }
    
  void BinaryData::writeString(const char * s)
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

  void BinaryData::writeVector2Float32(Nimble::Vector2f v)
  {
    ensure(12);
    getRef<int32_t>() = VECTOR2F_MARKER;
    getRef<float>() = v[0];
    getRef<float>() = v[1];
  }

  void BinaryData::append(const BinaryData & that)
  {
    ensure(that.pos());
    memcpy(getPtr<char>(that.pos()), that.data(), that.pos());
  }

  float BinaryData::readFloat32(bool * ok)
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

  int32_t BinaryData::readInt32(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      unavailable("BinaryData::readInt32");
      return 0;
    }

    int32_t marker = getRef<int32_t>();

    if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(marker == INT64_MARKER)
      return getRef<int64_t>();
    else if(marker == FLOAT_MARKER)
      return Nimble::Math::Round(getRef<float>());
    else if(ok) {
      badmarker("BinaryData::readInt32", marker);
      *ok = false;
    }

    skipParameter(marker);
    return 0;
  }

  int64_t BinaryData::readInt64(bool * ok)
  {
    if(!available(8)) {
      if(ok) * ok = false;
      unavailable("BinaryData::readInt64");
      return 0;
    }

    int64_t marker = getRef<int64_t>();

    if(marker == INT64_MARKER)
      return getRef<int64_t>();
    else if(marker == INT32_MARKER)
      return getRef<int32_t>();
    else if(marker == FLOAT_MARKER)
      return Nimble::Math::Round(getRef<float>());
    else if(ok) {
      badmarker("BinaryData::readInt64", marker);
      *ok = false;
    }

    skipParameter(marker);
    return 0;
  }

  int64_t BinaryData::readTimeStamp(bool * ok)
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
  
  bool BinaryData::readString(char * str, int maxbytes)
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

  Nimble::Vector2f BinaryData::readVector2Float32(bool * ok)
  {
    if(!available(12)) {
      if(ok) * ok = false;
      return Nimble::Vector2f(0, 0);
    }

    int32_t marker = getRef<int32_t>();

    if(marker != VECTOR2F_MARKER) {
      skipParameter(marker);
      if(ok)
	*ok = false;
      return Nimble::Vector2f(0, 0);
    }
    
    return getRef<Nimble::Vector2f>();
  }

  bool BinaryData::write(Radiant::BinaryStream * stream)
  {
    int32_t s = pos();
    if(stream->write(&s, 4) != 4)
      return false;

    return stream->write( & m_buf[0], s) == s;
  }

  bool BinaryData::read(Radiant::BinaryStream * stream)
  {
    uint32_t s = 0;
    m_current = 0;
    m_total = 0;

    if(stream->read(&s, 4) != 4)
      return false;

    if(m_buf.size() < s) {
      if(s > 500000000) { // Not more than 500 MB at once, please
	error("BinaryData::read # Attempting extraordinary read (%d bytes)",
	      s);
	return false;
      }
      m_buf.resize(s);
    }
    
    if(stream->read( & m_buf[0], s) != (int) s)
      return false;

    m_current = 0;
    m_total = s;

    return true;
  }

  void BinaryData::ensure(unsigned bytes)
  {
    unsigned need = m_current + bytes;
    if(need > m_buf.size()) {
      m_buf.resize(need + 128 + need / 16);
    }
    m_total = m_current + bytes;
  }

  void BinaryData::skipParameter(int marker)
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
  
  int BinaryData::stringSpace(const char * str)
  {
    int len = strlen(str) + 1;

    int rem = len & 0x3;

    int pad = rem ? 4 - rem : 0;
    return len + pad;
  }

}
