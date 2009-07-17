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

#include "UDPSocket.hpp"
#include "TCPSocket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>

namespace Radiant
{

  class UDPSocket::D {
    public:
      D() : m_fd(-1), m_port(0) {}

      int m_fd;
      int m_port;
      std::string m_host;
  };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  UDPSocket::UDPSocket()
  {
    m_d = new D();
  }

  UDPSocket::UDPSocket(int fd)
  {
    m_d = new D();
    m_d->m_fd = fd;
  }

  UDPSocket::~UDPSocket()
  {
    delete m_d;
  }

  int UDPSocket::open(const char * host, int port)
  {
    close();
   
    m_d->m_host = host;
    m_d->m_port = port;

    m_d->m_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_d->m_fd < 0)
      return errno;

    struct sockaddr_in server_address;

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short)port);

    in_addr * addr = TCPSocket::atoaddr(host);

    if(!addr)
      return EHOSTUNREACH;

    server_address.sin_addr.s_addr = addr->s_addr;

    if(connect(m_d->m_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
      return errno;

    return 0;
  }

  bool UDPSocket::close()
  {
    if(m_d->m_fd < 0)
      return false;

    ::close(m_d->m_fd);

    m_d->m_fd = -1;

    return true;
  }

  bool UDPSocket::isOpen() const
  {
    return (m_d->m_fd > 0);
  }

  int UDPSocket::read(void * buffer, int bytes)
  {
    if(m_d->m_fd < 0) 
      return -1;

    return ::read(m_d->m_fd, buffer, bytes);
  }

  int UDPSocket::write(const void * buffer, int bytes)
  {
    if(m_d->m_fd < 0)
      return -1;

    return ::write(m_d->m_fd, buffer, bytes);
  }

}
