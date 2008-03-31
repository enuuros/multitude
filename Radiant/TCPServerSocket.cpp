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

#include "TCPServerSocket.hpp"

#include "TCPSocket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace Radiant {


  TCPServerSocket::TCPServerSocket()
    : m_fd(-1)
  {}

  TCPServerSocket::~TCPServerSocket()
  {}

  int TCPServerSocket::open(const char * host, int port, int maxconnections)
  {
    close();

    m_host = host ? host : "";
    m_port = port;

    m_fd = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if(m_fd < 0){
      return errno;
    }

    struct sockaddr_in server_address;

    bzero( & server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if(!host || strlen(host) == 0)
      server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    else {
      in_addr * addr = TCPSocket::atoaddr(host);
      
      if(!addr)
	return EHOSTUNREACH;
      
      server_address.sin_addr.s_addr = addr->s_addr;
      // server_address.sin_addr.s_addr = inet_addr(host);
    }

    puts("Binding");

    if(bind(m_fd, ( struct sockaddr * ) & server_address,
	     sizeof ( server_address ) ) < 0 ){
      close();
      return errno;
    }

    puts("Listenging");
    
    if(::listen(m_fd, maxconnections) != 0) {
      close();
      return errno;
    }

    return 0;
  }

  bool TCPServerSocket::close()
  {
    if(m_fd < 0)
      return false;

    ::close(m_fd);

    m_fd = -1;

    return true;
  }
   
  bool TCPServerSocket::isPendingConnection(unsigned waitMicroSeconds)
  {
    if(m_fd < 0)
      return false;

    struct pollfd pfd;
    bzero( & pfd, sizeof(pfd));
    pfd.fd = m_fd;
    pfd.events = POLLIN;
    poll(&pfd, 1, waitMicroSeconds / 1000);
    return pfd.revents & POLLIN;
  }

  TCPSocket * TCPServerSocket::accept()
  {
    sockaddr newAddress;
    socklen_t addressLength(sizeof(newAddress));

    int fd = ::accept(m_fd, (sockaddr *) & newAddress, & addressLength);

    if(fd < 0)
      return 0;

    TCPSocket * sock = new TCPSocket(fd);
    
    return sock;
  }

} 
