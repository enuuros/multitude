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

#ifndef RADIANT_SM_PIPE_HPP
#define RADIANT_SM_PIPE_HPP

#include <Radiant/BinaryData.hpp>
#include <Radiant/Export.hpp>

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <WinPort.h>
#else
#	include <sys/ipc.h> // key_t on OSX
#endif

#include <string>

namespace Radiant
{

  /** One-directional shared-memory data pipe. */
  class RADIANT_API SMPipe
  {
  public:

    /// Constructor.
#ifdef WIN32
    /// @param smName User-defined name for shared memory.
    /// @param size Size in bytes of the ring buffer: if size > 0,
    /// creates a new ring buffer of that size; if size == 0,
    /// references the existing buffer identified by smKey.
    SMPipe::SMPipe(const std::string smName, const uint32_t size);
#else
    /// @param smKey User-defined key to shared memory.
    /// @param size Size in bytes of the ring buffer: if size > 0, creates a new ring buffer
    /// of that size; if size == 0, references the existing buffer identified by smKey.
    SMPipe(const key_t smKey, const uint32_t size);
#endif

    /// Destructor.
    virtual ~SMPipe();

    // Reads data from the buffer.
    int read(void * ptr, int n);
    int read(BinaryData &);
    uint32_t readAvailable();

    /// Stores data into the buffer, without flushing it.
    int write(const void * ptr, int n);
    int write(const BinaryData &);
    uint32_t writeAvailable(int require = 0);
    /// Flush the written data to the buffer
    void flush() { storeHeaderValue(SHM_WRITE_LOC, m_written); }
    

    uint32_t size() const { return m_size; }

  private:

    static uint32_t smDefaultPermissions();

    enum {
      SHM_SIZE_LOC = 0,
      SHM_WRITE_LOC  = 4,
      SHM_READ_LOC = 8,
      SHM_PIPE_LOC = 12,
      SHM_HEADER_SIZE = SHM_PIPE_LOC
    };

    /// Access functions.

    /// Return the read position.
    uint32_t readPos() const
    {
      return * ((uint32_t *)(m_shm + SHM_READ_LOC));
    }

    /// Return the read position.
    uint32_t writePos() const
    {
      return * ((uint32_t *)(m_shm + SHM_WRITE_LOC));
    }
    
    void storeHeaderValue(int loc, uint32_t val)
    { * ((uint32_t *)(m_shm + loc)) = val; }

    /// Clear and reset the ring buffer.
    // void clear() { setWritePos(0); setReadPos(0); setReadWriteState(RWS_NONE); }


    /// Diagnostics.

#ifndef WIN32
    /// Return error message for the most recent shared memory function error.
    static std::string shmError();
#endif

    /// Output attributes and properties.
    void dump() const;

    /// true if this is the creator object, false if it is a reference object.
    bool    m_isCreator;

#ifdef WIN32
    /// User-defined name for the shared memory area.
    std::string   m_smName;

    /// Handle to shared memory area.
    HANDLE  m_hMapFile;
#else
    /// User-defined key to the shared memory area.
    key_t   m_smKey;

    /// ID of the shared memory area.
    int     m_id;
#endif

    uint32_t m_size;
    uint32_t m_written;
    uint32_t m_read;
    uint32_t m_mask;
    // Pointer the actual shared memory:
    uint8_t  * m_shm;
    // Pointer to the pipe area:
    uint8_t  * m_pipe;

  };

}

#endif