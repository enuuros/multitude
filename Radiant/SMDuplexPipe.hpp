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

#ifndef RADIANT_SM_DUPLEX_PIPE_HPP
#define RADIANT_SM_DUPLEX_PIPE_HPP

#include <Radiant/SMPipe.hpp>

namespace Radiant
{
  
  /** Ful-duplex shared memory data pipe. This utility class packs two
      #SMPipe objects into one object. */
  class RADIANT_API SMDuplexPipe
  {
  public:
    SMDuplexPipe(const key_t writeKey, const uint32_t writeSize,
		 const key_t readKey,  const uint32_t readSize);
    virtual ~SMDuplexPipe();

    int read(void * ptr, int n) { return m_in.read(ptr, n); }
    int read(BinaryData & bd) { return m_in.read(bd); }
    uint32_t readAvailable() { return m_in.readAvailable(); }

    int write(const void * ptr, int n) { return m_out.write(ptr, n); }
    int write(const BinaryData & bd, bool doflush)
    { int n = m_out.write(bd); if(doflush) flush(); return n ; }
    uint32_t writeAvailable() { return m_out.writeAvailable(); }
    /// Flush the written data to the buffer
    void flush() { m_out.flush(); }

  private:
    SMPipe m_out;
    SMPipe m_in;
  };
  
}

#endif


