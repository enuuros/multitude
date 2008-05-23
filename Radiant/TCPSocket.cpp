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

#ifndef WIN32
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#else
#include <winsock2.h>
#include <WinPort.h>
#endif

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

#ifndef WIN32
    ::close(m_fd);
#else
	 ::closesocket((SOCKET)m_fd);
#endif

    m_fd = -1;

    return true;
  }
 
  int TCPSocket::read(void * buffer, int bytes)
  {
    if(m_fd < 0)
      return -1;

#ifndef WIN32
	 return ::read(m_fd, buffer, bytes);
#else
	 return ::recv((SOCKET)m_fd, (char*)buffer, bytes, 0);
#endif
  }

  int TCPSocket::write(const void * buffer, int bytes)
  {
    if(m_fd < 0)
      return -1;

#ifndef WIN32
    return ::write(m_fd, buffer, bytes);
#else
	 return ::send((SOCKET)m_fd, (char*)buffer, bytes, 0);
#endif
  }
  
  bool TCPSocket::isHungUp() const
  {
    if(m_fd < 0)
      return false;

#ifndef WIN32
    struct pollfd pfd;
    pfd.fd = m_fd;
    pfd.events = ~0;
    poll(&pfd, 1, 0);
    return pfd.revents & (POLLERR | POLLHUP | POLLNVAL) != 0;
#else
	// -- emulate using select()
	struct timeval timeout;
	timeout.tv_sec = timeout.tv_usec = 0;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_fd, &readfds);
	int status = select(m_fd, &readfds, 0,0, &timeout);
	if (status < 0)
		return true;
	char data;
	return (FD_ISSET(m_fd, &readfds) && (recv(m_fd, &data, 1, MSG_PEEK) <= 0));
#endif
  }

  bool TCPSocket::isPendingInput(unsigned waitMicroSeconds)
  {
    if(m_fd < 0)
      return false;

#ifndef WIN32
    struct pollfd pfd;
    pfd.fd = m_fd;
    pfd.events = POLLIN;
    poll(&pfd, 1, waitMicroSeconds / 1000);
    return pfd.revents & POLLIN;
#else
	// -- emulate using select()
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = waitMicroSeconds;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(m_fd, &readfds);
	int status = select(m_fd, &readfds, 0,0, &timeout);
	if (status < 0)
		return false;
	char data;
	return !(FD_ISSET(m_fd, &readfds) && (recv(m_fd, &data, 1, MSG_PEEK) <= 0));
#endif
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
