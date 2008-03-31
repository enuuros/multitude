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

#include "TCPSocket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace Radiant {

  TCPSocket::TCPSocket()
    : m_fd(-1),
      m_port(0)
  {}

  TCPSocket::TCPSocket(int fd)
    : m_fd(fd)
  {}

  TCPSocket::~TCPSocket()
  {}

  int TCPSocket::open(const char * host, int port)
  {
    close();

    m_host = host;
    m_port = port;

    m_fd = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if(m_fd < 0){
      return errno;
    }

    struct sockaddr_in server_address;

    bzero( & server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short) port);
    // server_address.sin_port = port;

    in_addr * addr = TCPSocket::atoaddr(host);

    if(!addr)
      return EHOSTUNREACH;

    server_address.sin_addr.s_addr = addr->s_addr;
    // server_address.sin_addr.s_addr = inet_addr(host);
    // server_address.sin_addr.s_addr = inet_addr(host);
    
    if(connect(m_fd, (struct sockaddr *) &server_address, 
	       sizeof(server_address)) < 0)
      return errno;
    
    return 0;
  }

  bool TCPSocket::close()
  {
    if(m_fd < 0)
      return false;

    ::close(m_fd);

    m_fd = -1;

    return true;
  }
 
  int TCPSocket::read(void * buffer, int bytes)
  {
    if(m_fd < 0)
      return -1;

    return ::read(m_fd, buffer, bytes);
  }

  int TCPSocket::write(const void * buffer, int bytes)
  {
    if(m_fd < 0)
      return -1;

    return ::write(m_fd, buffer, bytes);
  }
  
  bool TCPSocket::isHungUp() const
  {
    if(m_fd < 0)
      return false;
    
    struct pollfd pfd;
    pfd.fd = m_fd;
    pfd.events = ~0;
    poll(&pfd, 1, 0);
    return pfd.revents & (POLLERR | POLLHUP | POLLNVAL) != 0;
  }

  bool TCPSocket::isPendingInput(unsigned waitMicroSeconds)
  {
    if(m_fd < 0)
      return false;

    struct pollfd pfd;
    pfd.fd = m_fd;
    pfd.events = POLLIN;
    poll(&pfd, 1, waitMicroSeconds / 1000);
    return pfd.revents & POLLIN;
  }

  bool TCPSocket::setNoDelay(bool noDelay)
  {    
    int yes = noDelay;

    if (setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, (char *) & yes, 
		   sizeof(int))) {
      return false;
    }
    
    return true;
  }

  /* Converts ascii text to in_addr struct.  NULL is returned if the address
     can not be found. */
  struct in_addr * TCPSocket::atoaddr(const char *address)

  {
    struct hostent *host;
    static struct in_addr saddr;
    
    /* First try it as aaa.bbb.ccc.ddd. */
    saddr.s_addr = inet_addr(address);
    if ((int) saddr.s_addr != -1) {
      return &saddr;
    }
    host = gethostbyname(address);
    if (host != NULL) {
      return (struct in_addr *) *host->h_addr_list;
    }
    return NULL;
  }

} 
