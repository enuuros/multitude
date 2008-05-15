/* COPYRIGHT
 *
 * This file is part of Notable.
 *
 * Copyright: MultiTouch Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2008.
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from MultiTouch Oy.
 *
 * See file "Notable.hpp" for authors and more details.
 *
 */

#include "SMRingBuffer.hpp"

#include "Nimble/Math.hpp"

#include <errno.h>

namespace Radiant
{

  // Class SMRingBuffer.


  // Static data initialization.

  // rw-rw-rw-.
  uint32_t  SMRingBuffer::smDefaultPermissions = 0666;

  uint32_t  SMRingBuffer::smHeaderSize = sizeof(uint32_t) * 3;

  // Set maximum buffer size to the largest possible value of a 32-bit integer
  // less (header size + 1).
  uint32_t  SMRingBuffer::maxSize = 4294967295 - (smHeaderSize + 1);

  // Construction / destruction.

  SMRingBuffer::SMRingBuffer(const key_t smKey, const uint32_t size)
  : m_smKey(smKey)
  , m_id(-1)
  , m_startPtr(0)
  {
    trace("SMRingBuffer::SMRingBuffer");

    uint32_t  smSize = 0;
    bool      creator = false;

    if(size > 0)
    // Create new shared memory area (SMA)
    {
      // Validate requested size

      if(size > maxSize)
      {
        error("SMRingBuffer::SMRingBuffer: requested size %ul is greater than maximum size %ul.",
          (unsigned long)(size), (unsigned long)(maxSize));
        assert(0);
      }

      // Clear any existing SMA with this key

      const int   id = shmget(m_smKey, 0, smDefaultPermissions);
      if(id != -1)
      {
        if(shmctl(id, IPC_RMID, 0) != -1)
        {
          trace("SMRingBuffer::SMRingBuffer: successfully removed existing shared memory area with same key.");
        }
        else
        {
          error("SMRingBuffer::SMRingBuffer: unable to remove existing shared memory area with same key (%s).", shmError().c_str());
          assert(0);
        }
      }

      // Create the new SMA

      // shmget() rounds up size to nearest page size, so actual size of area may be greater
      // than requested size - however, this does not affect anything, the extra will simply
      // remain unused
      smSize = smHeaderSize + size + 1;
      m_id = shmget(m_smKey, smSize, smDefaultPermissions | IPC_EXCL | IPC_CREAT);
      if(m_id != -1)
      {
        creator = true;
        trace("SMRingBuffer::SMRingBuffer: successfully created new shared memory area.");
      }
      else
      {
        error("SMRingBuffer::SMRingBuffer: unable to create new shared memory area (%s).", shmError().c_str());
        assert(0);
      }
    }
    else
    // Try to reference existing SMA
    {
      m_id = shmget(m_smKey, 0, smDefaultPermissions);
      if(m_id != -1)
      {
        trace("SMRingBuffer::SMRingBuffer: successfully accessed existing shared memory area.");
      }
      else
      {
        error("SMRingBuffer::SMRingBuffer: unable to access existing shared memory area (%s).", shmError().c_str());
        assert(0);
      }
    }

    // Get pointer to SMA

    char * const    smPtr = (char *)(shmat(m_id, 0, 0));
    if(smPtr != (char *)(-1))
    {
      trace("SMRingBuffer::SMRingBuffer: successfully obtained pointer to shared memory area.");
    }
    else
    {
      error("SMRingBuffer::SMRingBuffer: unable to obtain pointer to shared memory area (%s)", shmError().c_str());
      assert(0);
    }

    if(creator)
    // This is the creating object
    {
      // initialize the SMA
      memset(smPtr, 0, smSize);

      // write headersize
      memcpy(smPtr, & size, sizeof(uint32_t));
    }

    // Set ring buffer pointer - buffer starts after header information

    m_startPtr = (unsigned char *)(smPtr) + smHeaderSize;

    assertValid();
  }

  SMRingBuffer::~SMRingBuffer()
  {
    trace("SMRingBuffer::~SMRingBuffer");

    assertValid();

    // Detach the SMA

    char * const  smPtr = (char *)(m_startPtr - smHeaderSize);
    if(shmdt(smPtr) != -1)
    {
      trace("SMRingBuffer::~SMRingBuffer: successfully detached shared memory area.");
    }
    else
    {
      error("SMRingBuffer::~SMRingBuffer: unable to detach shared memory area (%s).", shmError().c_str());
    }

    // Destroy the SMA (it will actually be destroyed only after the last detach,
    // i.e. when no more objects are referencing it).

    if(shmctl(m_id, IPC_RMID, 0) != -1)
    {
      trace("SMRingBuffer::~SMRingBuffer: successfully destroyed shared memory area.");
    }
    else
    {
      error("SMRingBuffer::SMRingBuffer: unable to destroy shared memory area (%s).", shmError().c_str());
    }
  }


  // Properties.

  uint32_t SMRingBuffer::used(uint32_t * const first) const
  {
    // Compute total bytes used

    const uint32_t  bufSize = size();
    const uint32_t  wPos = writePos();
    const uint32_t  rPos = readPos();
    const uint32_t  total = (rPos <= wPos) ? wPos - rPos : (bufSize + wPos) - rPos;

    if(first)
    {
      // If used part of buffer wraps, compute size of first block
      * first = ((rPos + total) > bufSize) ? bufSize - rPos : total;
      // second = total - first
    }

    return total;
  }

  uint32_t SMRingBuffer::available(uint32_t * const first) const
  {
    // Compute total bytes available

    const uint32_t  bufSize = size();
    const uint32_t  wPos = writePos();
    const uint32_t  rPos = readPos();
    const uint32_t  total = (wPos < rPos) ? (rPos - wPos) - 1 : ((bufSize + rPos) - wPos) - 1;

    if(first)
    {
      // If available part of buffer wraps, compute size of first block
      * first = ((wPos + total) >= bufSize) ? bufSize - wPos : total;
      // second = total - first
    }

    return total;
  }


  // Reading and writing.

  bool SMRingBuffer::write(const void * const src, const uint32_t numBytes)
  {
//    trace("SMRingBuffer::write");

    assert(src);
    assert(numBytes > 0);

    // Check sufficient space available

    uint32_t         firstAvl = 0;
    const uint32_t   totalAvl = available(& firstAvl);

    if(numBytes > totalAvl)
    {
//      error("SMRingBuffer::write: insufficient space, %ul requested, %ul available.",
//        (unsigned long)(numBytes), (unsigned long)(totalAvl));
      return false;
    }

    // Write the data

    unsigned char * const    wPtr = writePtr();

    if(numBytes <= firstAvl)
    // in 1 chunk
    {
      memcpy(wPtr, src, numBytes);
    }
    else
    // in 2 chunks
    {
      memcpy(wPtr, src, firstAvl);
      memcpy(m_startPtr, (unsigned char *)(src) + firstAvl, numBytes - firstAvl);
    }

    advanceWritePos(numBytes);

    assertValid();

    return true;
  }

  bool SMRingBuffer::peek(void * const dst, const uint32_t numBytes)
  {
    const uint32_t  prevReadPos = readPos();
    const bool      result = read(dst, numBytes);
    setReadPos(prevReadPos);

    assertValid();

    return result;
  }

  bool SMRingBuffer::read(void * const dst, const uint32_t numBytes)
  {
//    trace("SMRingBuffer::read");

    assert(dst);
    assert(numBytes > 0);

    // Check read area does not overlap write position

    uint32_t         firstUsd = 0;
    const uint32_t   totalUsd = used(& firstUsd);

    if(numBytes > totalUsd)
    {
//     error("SMRingBuffer::read: insufficient data, %ul requested, %ul in use.",
//        (unsigned long)(numBytes), (unsigned long)(totalUsd));
      return false;
    }

    // Read the data

    const unsigned char * const  rPtr = readPtr();

    if(numBytes <= firstUsd)
    // in 1 chunk
    {
      memcpy(dst, rPtr, numBytes);
    }
    else
    // in 2 chunks
    {
      memcpy(dst, rPtr, firstUsd);
      memcpy((unsigned char *)(dst) + firstUsd, m_startPtr, numBytes - firstUsd);
    }

    advanceReadPos(numBytes);

    assertValid();

    return true;
  }

  bool SMRingBuffer::discard(const uint32_t numBytes)
  {
    trace("SMRingBuffer::discard");

    assert(numBytes > 0);

    // Check discard area does not overlap write position

    const uint32_t   totalDis = used();
    if(numBytes > totalDis)
    {
      error("SMRingBuffer::discard: insufficient data.");
      return false;
    }

    // Discard the data

    advanceReadPos(numBytes);

    assertValid();

    return true;
  }


  // Diagnostics.

  std::string SMRingBuffer::shmError()
  {
    std::string   errMsg;

    switch(errno)
    {
      case EACCES: errMsg = "EACCES"; break;

      case EEXIST: errMsg = "EEXIST"; break;

      case EFAULT: errMsg = "EFAULT"; break;

      case EIDRM:  errMsg = "EIDRM";  break;

      case EINVAL: errMsg = "EINVAL"; break;

      case EMFILE: errMsg = "EMFILE"; break;

      case ENOENT: errMsg = "ENOENT"; break;

      case ENOMEM: errMsg = "ENOMEM"; break;

      case ENOSPC: errMsg = "ENOSPC"; break;

      case EPERM:  errMsg = "EPERM";  break;

      default: assert(0);
    }

    return errMsg;
  }

  void SMRingBuffer::assertValid() const
  {
    assert(m_smKey > 0);
    assert(m_startPtr);

    const uint32_t    bufSize = size();
    assert(writePos() < bufSize);
    assert(readPos() < bufSize);
  }

  void SMRingBuffer::dump() const
  {
    trace("SMRingBuffer::dump()");

    trace("m_smKey = %ul", (unsigned long)(m_smKey));
    trace("m_id = %d", m_id);
    trace("size() = %ul", (unsigned long)(size()));
    trace("m_startPtr = %p", m_startPtr);

    trace("writePos() = %ul", (unsigned long)(writePos()));
    trace("readPos() = %ul", (unsigned long)(readPos()));

    trace("used = %ul", (unsigned long)(used()));
    trace("available() = %ul", (unsigned long)(available()));

    trace("isEmpty() = %s", isEmpty() ? "true" : "false");
    trace("isFull() = %s", isFull() ? "true" : "false");
  }

}
