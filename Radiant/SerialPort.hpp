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

#ifndef RADIANT_SERIAL_PORT_HPP
#define RADIANT_SERIAL_PORT_HPP

#include <stdint.h>
#include <unistd.h>

namespace Radiant
{

  /// A serial port handler. 
  /** This class is for binary IO with serial ports.

      @author Tommi Ilmonen
  */

  class SerialPort
  {
  public:
    SerialPort();
    /// Delete the objec and close the port
    ~SerialPort();
  
    /// Opens a serial port for communications
    /** If the port was open, this method will close it before opening
	it. */
    bool open(const char * device, 
	      /// Use stop bit
	      bool stopBit, 
	      /// Use parity bit
	      bool parityBit,
	      /// The baud rate
	      int baud, 
	      /// The number of data bits
	      int bits, 
	      /// The number of bytes to be read before returning
	      int waitBytes, 
	      /// Time to wait in microseconds
	      int waitTimeUS);
    /// Close the serial port.
    bool close();

    /// Write bytes to the port
    /** This method returns the number of bytes written. */
    int write(const void * buf, int bytes);
    int writeByte(uint8_t byte)
    { return write( & byte, 1); }
    /// Read bytes from the port
    /** This method returns the number of bytes read. */
    int read(void * buf, int bytes);

    inline bool isOpen() const { return m_fd >= 0; }
  
  protected:
  
    int m_fd;
  };

}

#endif


