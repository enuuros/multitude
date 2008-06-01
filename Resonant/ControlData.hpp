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

#ifndef RESONANT_CONTROL_DATA_HPP
#define RESONANT_CONTROL_DATA_HPP

#include <Radiant/BinaryData.hpp>

//#include <stdint.h>

//#include <vector>

namespace Resonant {

  /// OSC-like control messages
  /** This class encapsulates control messages in a binary buffer. The
      data is stored in pretty much the same way as in Open Sound
      Control (OSC). This is done to ensure easy control data
      conversion between Resonant and OSC-compliant applications.
      
      \b Differences between Resonant and OSC:
      
      <UL>

      <LI>In ControlData the byte order is machine native byte
      order, for performance reasons.

      <LI> Time-stamp for is slightly different. In Resonant the the
      timestamps are 40+24 bits fixed point while in OSC they are
      32+32 bit fixed point.

      <LI> In Resonant, there is no address matching

      <LI> In Resonant, the type of each parameter is stored right
      before the parameter, to make writing/reading the stream easier.

      <LI> In Resonant strings are padded to 4-byte margin (like OSC),
      but the padding does not need to be zeros.

      </UL>

      OSC basic types are well known, so automatic conversion of all the
      differences should be easy enough.

      \b Wrting functions always put the type marker before the actual
      value.

      \b Reading functions set the optional bool argument "ok" to
      false if the operation fails. The boolean is never set to true,
      so you must do that in our own code.
      
  */
  /*
  class ControlData
  {
  public:
    ControlData();
    ~ControlData();

    /// Writes a 32-bit floating point number to the data buffer
    void writeFloat32(float v);
    /// Writes a 32-bit integer to the data buffer
    void writeInt32(int32_t v);
    /// Writes a 64-bit time-stamp to the data buffer
    void writeTimeStamp(int64_t v);
    /// Write a null-terminated string to the buffer
    void writeString(const char *);

    /// Reads a 32-bit floating point number from the data buffer
    float readFloat32(bool * ok = 0);
    /// Reads a 32-bit integer from the data buffer
    int32_t readInt32(bool * ok = 0);
    /// Reads a 64-bit time-stamp from the data buffer
    int64_t readTimeStamp(bool * ok = 0);
    /// Read a null-terminated string from the buffer
    bool readString(char * str, int maxbytes);

    /// Tells the current position of the read/write pointer
    inline int pos() const { return m_current; } 
    /// Sets the position of the read/write pointer
    inline void setPos(int index) { m_current = index; }
    /// Rewind the index pointer to the beginning
    inline void rewind() { m_current = 0; }

    inline int total() const { return m_total; }

  protected:

    template <class T> 
    inline T * getPtr(int advance = sizeof(T))
    { T * tmp = (T*) & m_buf[m_current]; m_current += advance; return tmp; }
    template <class T>
    inline T & getRef()
    { T * tmp = (T*) & m_buf[m_current]; m_current += sizeof(T); return *tmp; }

    void ensure(unsigned bytes);
    inline bool available(unsigned bytes)
    { return (m_current + bytes) <= m_total; }
    void skipParameter(int marker);
    int stringSpace(const char * str);
    
    unsigned m_current;
    unsigned m_total;
    std::vector<char> m_buf;
  };
*/
  
  class ControlData : public Radiant::BinaryData
  {};
}

#endif
