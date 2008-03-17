/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DIVA_TCP_SOCKET_HPP
#define DIVA_TCP_SOCKET_HPP

#include <Radiant/BinaryStream.hpp>

#include <string>

struct in_addr;

namespace Radiant {

  /// A client TCP socket for connecting to remote hosts
  /** @author Tommi Ilmonen*/
  class TCPSocket : public Radiant::BinaryStream
  {
  public:
    TCPSocket();
    /// Construct a socket based on a file descriptor
    /** This method is potentially non-portable as not all platforms
        use file descriptors to handle sockets. */
    TCPSocket(int fd);
    ~TCPSocket();

    /// Opens a TCP socket to desired host:port
    /** @return On successful execution, returns zero, otherwise an
        error code (as in errno.h). */
    int open(const char * host, int port);
    /// Closes the socket
    bool close();

    /// Returns true of the socket is open.
    bool isOpen() { return m_fd > 0; }

    const char * host() const { return m_host.c_str(); }
    int port() const { return m_port; }

    /// Read bytes from the socket
    int read(void * buffer, int bytes);
    /// Write bytes to the socket
    int write(const void * buffer, int bytes);
    
    /// Returns true if the socket has been closed
    bool isHungUp() const;

    /// Return 'true' if readable data is pending.
    bool isPendingInput(unsigned waitMicroSeconds = 0);

    /// Sets the socket to no-delay mode
    /** Usually the operating system collects data before transmitting
	it. This is done to increase performance in situations where
	the application write many small packets to the socket. If the
	latency of the packets is critical, then calling
	setNoDelay(true) will disable this feature. */
    bool setNoDelay(bool noDelay);
    
    /// Convert an IP address to in_addr structure
    static struct in_addr *atoaddr(const char *address);
    
  protected:
    TCPSocket(const TCPSocket & ) : BinaryStream() {}
    TCPSocket & operator = (const TCPSocket & )  { return * this; }

    int m_fd;
    int m_port;
    std::string m_host;
  };

}

#endif
