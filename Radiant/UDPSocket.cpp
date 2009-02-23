#include "UDPSocket.hpp"
#include "TCPSocket.hpp"
#include <string.h>

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

namespace Radiant
{

  UDPSocket::UDPSocket()
    : m_fd(-1),
    m_port(0)
  {}

  UDPSocket::UDPSocket(int fd)
    : m_fd(fd)
  {}

  UDPSocket::~UDPSocket()
  {}

  int UDPSocket::open(const char * host, int port)
  {
    close();

    m_host = host;
    m_port = port;

    m_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(m_fd < 0)
      return errno;

    struct sockaddr_in server_address;

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short)port);

    in_addr * addr = TCPSocket::atoaddr(host);

    if(!addr)
      return EHOSTUNREACH;

    server_address.sin_addr.s_addr = addr->s_addr;

    if(connect(m_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
      return errno;

    return 0;
  }

  bool UDPSocket::close()
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

  int UDPSocket::read(void * buffer, int bytes)
  {
    if(m_fd < 0) 
      return -1;

#ifndef WIN32
    return ::read(m_fd, buffer, bytes);
#else
    return ::recv((SOCKET)m_fd, (char *)buffer, bytes, 0);
#endif
  }

  int UDPSocket::write(const void * buffer, int bytes)
  {
    if(m_fd < 0)
      return -1;

#ifndef WIN32
    return ::write(m_fd, buffer, bytes);
#else
    return ::send((SOCKET)m_fd, (char *)buffer, bytes, 0);
#endif
  }
}
