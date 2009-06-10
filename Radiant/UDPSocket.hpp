#ifndef RADIANT_UDP_SOCKET_HPP
#define RADIANT_UDP_SOCKET_HPP

#include <Radiant/BinaryStream.hpp>

#include <string>

namespace Radiant
{
  
  /** UPD socket implementation. */
  class RADIANT_API UDPSocket : public Radiant::BinaryStream
  {
    public:
      UDPSocket();
      UDPSocket(int fd);
      ~UDPSocket();

      int open(const char * host, int port);
      bool close();

      bool isOpen() const;

      int read(void * buffer, int bytes);
      int write(const void * buffer, int bytes);

    private:
      class D;
      D * m_d;
  };

}

#endif
