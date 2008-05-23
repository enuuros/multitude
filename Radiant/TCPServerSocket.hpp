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

#ifndef RADIANT_TCP_SERVER_SOCKET_HPP
#define RADIANT_TCP_SERVER_SOCKET_HPP

#include <string>

namespace Radiant {

  class TCPSocket;
  
  /// A server TCP socket for accepting incoming connections
  /** A serer socket */

  class EXPORT TCPServerSocket
  {
  public:
    TCPServerSocket();
    ~TCPServerSocket();

    /// Opens a server TCP socket to desired host:port
    /** @return On successful execution, returns zero, otherwise an
        error code (as in errno.h). */
    int open(const char * host, int port, int maxconnections = 1);
    /// Closes the socket
    bool close();
    /// Returns true of the socket is open.
    bool isOpen() { return m_fd > 0; }

    const char * host() const { return m_host.c_str(); }
    int port() const { return m_port; }

    /// Return 'true' if connection pending.
    bool isPendingConnection(unsigned waitMicroSeconds = 0);

    /// Accept new connections
    TCPSocket * accept();
    
  protected:
    TCPServerSocket(const TCPServerSocket & ) {}
    TCPServerSocket & operator = (const TCPServerSocket & ) { return * this; }

    int m_fd;
    int m_port;
    std::string m_host;
  };

}

#endif
