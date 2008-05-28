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

#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <Radiant/Trace.hpp>

#include <assert.h>

#include <memory>

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <string>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

namespace Radiant
{

  /**
    * @class SMRingBuffer
    *
    * This class implements a ring buffer in shared memory to facilitate the passing of
    * binary data between two processes.
    *
    * Notes
    *
    * 1. The write position is never allowed to catch up with the read position. This
    *    makes it possible to distinguish an empty buffer (read pos == write pos) from
    *    a full buffer (read pos == write pos + 1), at the cost of a redundant byte.
    *
    * Usage
    *
    * 1. Decide a unique key for the shared memory area (SMA). It can be any positive
    *    integer, e.g. 1234. Make the key available to all processes by defining it in
    *    a common header file.
    * 2. To create the ring buffer, construct a SMRingBuffer object with the unique key
    *    and required size as parameters. The constructor handles allocation of the
    *    shared memory.
    * 3. To refer to the ring buffer from the same or other process, construct other
    *    SMRingBuffer object(s) using the same key.
    * 4. Call write() to add data to the ring buffer, and read() to remove it.
    * 5. Processes themselves are responsible for formatting data to be written, e.g.
    *    prepending size or type information.
    * 6. The SMRingBuffer destructor handles deallocation of the shared memory.
    */
  class SMRingBuffer
  {


    public:


      /// Static data.

      /// Default permissions for shared memory.
      static uint32_t   smDefaultPermissions;

      /// Size of header information preceding ring buffer in shared memory.
      /// The header contains 3 items that must be available to all sharing processes:
      /// buffer size, write position and read position.
      static uint32_t   smHeaderSize;

      /// Maximum size in bytes of buffer.
      static uint32_t   maxSize;


      /// Construction / destruction.

      /// Constructor.
      /// @param smKey User-defined key to shared memory.
      /// @param size Size in bytes of the ring buffer: if size > zero, creates a new ring buffer
      /// of that size; if size == zero, references the existing buffer identified by smKey.
      SMRingBuffer(const key_t smKey, const uint32_t size = 0);

      /// Destructor.
      ~SMRingBuffer();


      /// Access functions.

      /// Return the size in bytes of the ring buffer.
      uint32_t size() const
      {
        return * ((uint32_t *)(m_startPtr - smHeaderSize));
      }

      /// Return the write position.
      uint32_t writePos() const
      {
        return * ((uint32_t *)((m_startPtr - smHeaderSize) + sizeof(uint32_t)));
      }

      /// Return the read position.
      uint32_t readPos() const
      {
        return * ((uint32_t *)((m_startPtr - smHeaderSize) + sizeof(uint32_t) * 2));
      }

      /// Set the write position.
      void setWritePos(const uint32_t writePos)
      {
        * ((uint32_t *)((m_startPtr - smHeaderSize) + sizeof(uint32_t))) = writePos;
      }

      /// Set the read position.
      void setReadPos(const uint32_t readPos)
      {
        * ((uint32_t *)((m_startPtr - smHeaderSize) + sizeof(uint32_t) * 2)) = readPos;
      }

      /// Properties.

      /// Return the size in bytes of the used part of the buffer.
      /// @param first Optional pointer to int32_t to receive the size of the first
      /// used block, in case of wrap.
      uint32_t used(uint32_t * const first = 0) const;

      /// Return the size in bytes of the available (unused) part of the buffer.
      /// @param first Optional pointer to int32_t to receive the size of the first
      /// available block, in case of wrap.
      uint32_t available(uint32_t * const first = 0) const;

      /// Return true if the ring buffer is empty.
      bool isEmpty() const { return (used() == 0); }

      /// Return true if the ring buffer is full.
      bool isFull() const { return (available() == 0); }

      /// Reading and writing.

      /// Write data to the ring buffer and (if successful) advance the write position.
      /// @param src Pointer to data source.
      /// @param numBytes Size of the data.
      /// @return true if successful, false otherwise.
      bool write(const void * const src, const uint32_t numBytes);

      /// Peek data in the ring buffer but do not advance the read position.
      /// @param dst Pointer to data destination.
      /// @param numBytes Number of bytes of data to peek.
      /// @return true if successful, false otherwise.
      bool peek(void * const dst, const uint32_t numBytes);

      /// Read data from the ring buffer and (if successful) advance the read position.
      /// @param dst Pointer to data destination.
      /// @param numBytes Number of bytes of data to read.
      /// @return true if successful, false otherwise.
      bool read(void * const dst, const uint32_t numBytes);

      /// Discard data from the ring buffer and (if successful) advance the read position.
      /// @param numBytes Number of bytes of data to discard.
      /// @return true if successful, false otherwise.
      bool discard(const uint32_t numBytes);

      /// Clear the ring buffer, reset write and read positions to zero.
      void clear() { setWritePos(0); setReadPos(0); }


      /// Diagnostics.

      /// Return error message for the most recent shared memory function error.
      static std::string shmError();

      /// Assert the validity of the ring buffer.
      void assertValid() const;

      /// Output attributes and properties.
      void dump() const;


    protected:


      /// Shared memory header access functions.

      /// Return a pointer to the write position.
      unsigned char * const writePtr() const { return (m_startPtr + writePos()); }

      /// Return a pointer to the read position.
      const unsigned char * const readPtr() const { return (m_startPtr + readPos()); }

      /// Advance the write position.
      void advanceWritePos(const uint32_t advance) { setWritePos((writePos() + advance) % size()); }

      /// Advance the read position.
      void advanceReadPos(const uint32_t advance) { setReadPos((readPos() + advance) % size()); }


      /// Attributes.
      /// @note The first smHeaderSize bytes of the shared memory area (SMA) are used
      /// to store the header information. Consequently the ring buffer start = SMA
      /// start + header size.

      /// User-defined key to the shared memory area.
      key_t     m_smKey;

      /// ID of the shared memory area.
      int       m_id;

      /// Pointer to the start of the ring buffer.
      unsigned char *   m_startPtr;

  };

}

#endif
