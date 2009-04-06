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

#include "SMPipe.hpp"

#include <Radiant/StringUtils.hpp>
#include <Radiant/Sleep.hpp>
#include <Radiant/TimeStamp.hpp>
#include <Radiant/Trace.hpp>

#include <Nimble/Math.hpp>

#include <cassert>
#include <cerrno>
#include <iostream>
#include <memory>
#include <sstream>

#include <string.h>

#ifndef WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#endif

namespace Radiant
{

  // Class SMPipe.


  // Static data initialization.

#ifdef WIN32
  uint32_t  SMPipe::smDefaultPermissions() { return PAGE_EXECUTE_READWRITE; }
#else
  // rw-rw-rw-.
  uint32_t  SMPipe::smDefaultPermissions() { return 0666; }
#endif

  // Set maximum buffer size to the largest possible value of a 32-bit integer less (header size + 1).
  // uint32_t  SMPipe::maxSize = 4294967295u - (smHeaderSize + 1);


  // Construction / destruction.

#ifdef WIN32
  SMPipe::SMPipe(const std::string smName, const uint32_t size)
    : m_isCreator(false),
      m_smName(smName),
      m_hMapFile(0),
      m_size(0),
      m_written(0),
      m_read(0),
      m_mask(0),
      m_shm(0),
      m_pipe(0)
  {
    const char * const  fnName = "SMPipe::SMPipe";

    if(size > 0)
    // Create new shared memory area (SMA)
    {
      // Validate requested size

      unsigned s = 1;

      while(s < size)
	s = s << 1;

      size = s;

      if(size > maxSize)
      {
        error("%s # Requested size %ul is greater than maximum size %ul.",
          fnName, (unsigned long)(size), (unsigned long)(maxSize));
        assert(0);
      }

      // Clear any existing SMA with this name

      const HANDLE  hMapFile = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, m_smName.c_str());
      if(hMapFile)
      {
        if(::CloseHandle(hMapFile)) {
          debug("%s # Successfully removed existing shared memory area with same name.", fnName);
        }
        else
        {
          error("%s # Failed to remove existing shared memory area with same name (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
          assert(0);
        }
      }

      // Create the new SMA

      m_hMapFile = ::CreateFileMappingA(INVALID_HANDLE_VALUE, 0, smDefaultPermissions, 0, size, m_smName.c_str());
      if(m_hMapFile) {
        m_isCreator = true;
        debug("%s # Successfully created new shared memory area (%s).", fnName);
      }
      else
      {
        error("%s # Failed to create new shared memory area (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
        assert(0);
      }
    }
    else
    // Try to reference existing SMA
    {
      m_hMapFile = ::OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, m_smName.c_str());
      if(m_hMapFile)
      {
        debug("%s # Successfully accessed existing shared memory area (%s).", fnName);
      }
      else
      {
        error("%s # Failed to access existing shared memory area (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
        assert(0);
      } 
    }

    // Get pointer to SMA

    char * const  smPtr = (char *)(::MapViewOfFile(m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size));
    if(smPtr)
    {
      debug("%s # Successfully obtained pointer to shared memory area.", fnName);
    }
    else
    {
      error("%s # Failed to obtain pointer to shared memory area (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
      assert(0);
    }

    if(m_isCreator)
    // This is the creating object
    {
      // initialize header
      memset(smPtr, 0, smHeaderSize);
      // write size to header
      memcpy(smPtr, & size, sizeof(uint32_t));
    }

    m_mask = m_size - 1;
    m_shm = (uint8_t *) smPtr;
    m_pipe = m_shm + SHM_PIPE_LOC;

    assert(isValid());
  }
#else
  SMPipe::SMPipe(const key_t smKey, const uint32_t size)
    : m_isCreator(false),
      m_smKey(smKey),
      m_id(-1),
      m_size(0),
      m_written(0),
      m_read(0),
      m_mask(0),
      m_shm(0),
      m_pipe(0)
  {
    const char * const  fnName = "SMPipe::SMPipe";

    if(size > 0)
    // Create new shared memory area (SMA)
    {
      // Validate requested size

      /* if(size > maxSize)
      {
        error("%s # Requested size %ul is greater than maximum size %ul.",
          fnName, (unsigned long)(size), (unsigned long)(maxSize));
        assert(0);
      }
      */

      // Clear any existing SMA with this key

      const int id = shmget(m_smKey, 0, smDefaultPermissions());
      if(id > 0) {
        if(shmctl(id, IPC_RMID, 0) != -1) {
	  debug("%s # Successfully removed existing shared memory area with same key.", fnName);
        }
        else {
          error("%s # Failed to remove existing shared memory area with same key (%s).", fnName, shmError().c_str());
          assert(0);
        }
      }

      // Create the new SMA

      /* shmget() rounds up size to nearest page size, so actual size
	 of area may be greater than requested size - however, this
	 does not affect anything, the extra will simply remain
	 unused */ 
      m_id = shmget(m_smKey, SHM_HEADER_SIZE + size + 1,
		    smDefaultPermissions() | IPC_EXCL | IPC_CREAT);
      if(m_id != -1) {
        m_isCreator = true;
	debug("%s # Successfully created new shared memory area.", fnName);
      }
      else {
        error("%s # Failed to create new shared memory area (%s).", fnName, shmError().c_str());
        assert(0);
      }
    }
    else
    // Try to reference existing SMA
    {
      m_id = shmget(m_smKey, 0, smDefaultPermissions());
      if(m_id != -1) {
       debug("%s # Successfully accessed existing shared memory area.", fnName);
      }
      else {
        error("%s # Failed to access existing shared memory area (%s).", fnName, shmError().c_str());
        assert(0);
      }
    }

    // Get pointer to SMA

    char * const  smPtr = (char *)(shmat(m_id, 0, 0));
    if(smPtr != (char *)(-1)) {
     debug("%s # Successfully obtained pointer to shared memory area.", fnName);
    }
    else {
      error("%s # Failed to obtain pointer to shared memory area (%s)", fnName, shmError().c_str());
      assert(0);
    }

    if(m_isCreator) {
      // This is the creating object

      // initialize header
      bzero(smPtr, SHM_HEADER_SIZE);
      // write size to header
      memcpy(smPtr, & size, sizeof(uint32_t));
      m_size = size;
    }
    else {
      memcpy( & m_size, smPtr, sizeof(uint32_t));
    }
    
    m_mask = m_size - 1;
    m_shm = (uint8_t *) smPtr;
    m_pipe = m_shm + SHM_PIPE_LOC;

    // assert(isValid());
  }
#endif

#ifdef WIN32
  SMPipe::~SMPipe()
  {
    const char * const  fnName = "SMPipe::~SMPipe";

    assert(isValid());

    // Detach the SMA

    char * const  smPtr = (char *)(m_shm - SHM_HEADER_SIZE);
    if(::UnmapViewOfFile(smPtr))
    {
     debug("%s # Successfully detached shared memory area.", fnName);
    }
    else
    {
      error("%s # Failed to detach shared memory area (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
    }

    // Only the creating object can destroy the SMA, after the last detach, i.e. when no more
    // objects are referencing it.

    if(m_isCreator) {
      if(::CloseHandle(m_hMapFile)) {
	debug("%s # Successfully destroyed shared memory area.", fnName);
      }
      else {
        error("%s # Failed to destroy shared memory area (%s).", fnName, StringUtils::getLastErrorMessage().c_str());
      }
    }
  }
#else
  SMPipe::~SMPipe()
  {
    const char * const  fnName = "SMPipe::~SMPipe";

    // assert(isValid());

    // Detach the SMA

    char * const  smPtr = (char *)(m_shm - SHM_HEADER_SIZE);
    if(shmdt(smPtr) != -1) {
     debug("%s # Successfully detached shared memory area.", fnName);
    }
    else {
      error("%s # Failed to detach shared memory area (%s).", fnName, shmError().c_str());
    }

    // Only the creating object can destroy the SMA, after the last detach, i.e. when no more
    // objects are referencing it.

    if(m_isCreator) {
      if(shmctl(m_id, IPC_RMID, 0) != -1) {
       debug("%s # Successfully destroyed shared memory area.", fnName);
      }
      else {
        error("%s # Failed to destroy shared memory area (%s).", fnName, shmError().c_str());
      }
    }
  }
#endif


  int SMPipe::read(void * ptr, int n)
  {
    // int orig = n;

    uint32_t avail = readAvailable();

    uint8_t * dest = (uint8_t *) ptr;
    const uint8_t  * pipe = m_pipe;

    n = Nimble::Math::Min((uint32_t) n, avail);
    int m = m_mask;
    for(int i = 0; i < n; i++) {
      dest[i] = pipe[m_read + i & m];
    }
    
    m_read += n;

    storeHeaderValue(SHM_READ_LOC, m_read);

    /* if(n)
      info("SMPipe::read # Read %d vs %d (%d vs %d)",
	   n, orig, readPos(), writePos());
    */

    return n;
  }

  int SMPipe::read(BinaryData & data)
  {
    uint32_t bytes = 0;

    int n = read( & bytes, 4);
    if(n != 4)
      return n;

    data.rewind();
    data.ensure(bytes);
    n = read(data.data(), bytes);
    data.setTotal(n);

    return n +  4;
  }

  uint32_t SMPipe::readAvailable()
  {
    uint32_t rp = readPos();
    uint32_t wp = writePos();
    
    return wp - rp;
  }

  int SMPipe::write(const void * ptr, int n)
  {
    int orig = n;

    uint32_t avail = writeAvailable();

    const uint8_t * src = (const uint8_t *) ptr;
    uint8_t  * pipe = m_pipe;

    n = Nimble::Math::Min((uint32_t) n, avail);
    
    int m = m_mask;
    for(int i = 0; i < n; i++) {
      pipe[m_written + i & m] = src[i];
    }

    m_written += n;

    /*if(n)
      info("SMPipe::write # Wrote %d vs %d (%d vs %d)",
	   n, orig, readPos(), writePos());
    */
    return n;
  }

  int SMPipe::write(const BinaryData & data)
  {
    if(!writeAvailable(data.pos() + 4)) {
      error("SMPipe::write # Not enough space in the pipe");
      return 0;
    }

    int bytes = data.pos();
    if(write(&bytes, 4) != 4)
      return 0;

    return write(data.data(), bytes) + 4;
  }

  uint32_t SMPipe::writeAvailable(int require)
  {
    uint32_t rp = readPos() + size();
    uint32_t wp = writePos();
    
    uint32_t avail = rp - wp;
    if(avail)
      avail--;

    if(require) {
      int times = 0;

      TimeStamp entry = TimeStamp::getTime();

      while((int) avail < require && times < 100) {

	/* if(!times) {
	  info("SMPipe::writeAvailable # Blocking");
	}
	*/
	uint32_t rp = readPos() + size();
	uint32_t wp = writePos();
    
	avail = rp - wp;
	if(avail)
	  avail--;

	Sleep::sleepMs(2);
	times++;
      }

      if(times) {
	TimeStamp now = TimeStamp::getTime();

	float spent = TimeStamp(now - entry).secondsD() * 1000.0;
	if(spent > 100) {
	  debug("SMPipe::writeAvailable # Blocked for %.3f avail %u vs %u",
		spent, avail, require);
	}
      }
    }
    
    return avail;
  }

  // Diagnostics.

#ifndef WIN32
  std::string SMPipe::shmError()
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

      default:
      {
        std::stringstream  ss;
        ss << errno;
        errMsg = std::string("errno = ") + ss.str();
      }
    }

    return errMsg;
  }
#endif

  void SMPipe::dump() const
  {
    debug("m_isCreator = %s", m_isCreator ? "true" : "false");
#ifdef WIN32
    debug("m_smName = %s", m_smName.c_str());
    debug("m_hMapFile = %p", m_hMapFile);
#else
    debug("m_smKey = %ul", (unsigned long)(m_smKey));
    debug("m_id = %d", m_id);
#endif
    debug("size() = %ul", (unsigned long)(size()));

    debug("writePos() = %ul", (unsigned long)(writePos()));
    debug("readPos() = %ul", (unsigned long)(readPos()));
    /*
    debug("readWriteState() = %ul", (unsigned long)(readWriteState()));

    debug("used = %ul", (unsigned long)(used()));
    debug("available() = %ul", (unsigned long)(available()));

    debug("isEmpty() = %s", isEmpty() ? "true" : "false");
    debug("isFull() = %s", isFull() ? "true" : "false");
    */
  }

}
