#ifndef RADIANT_UDP_SOCKET_HPP
#define RADIANT_UDP_SOCKET_HPP

#include <Radiant/BinaryStream.hpp>

#include <string>

namespace Radiant
{

  class RADIANT_API UDPSocket : public Radiant::BinaryStream
  {
    public:
      UDPSocket();
      UDPSocket(int fd);
      ~UDPSocket();

      int open(const char * host, int port);
      bool close();

      bool isOpen() { return m_fd > 0; }

      int read(void * buffer, int bytes);
      int write(const void * buffer, int bytes);

    private:
      int m_fd;
      int m_port;
      std::string m_host;
  };

}

#endif
