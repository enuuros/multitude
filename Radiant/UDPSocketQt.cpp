#include "UDPSocket.hpp"
#include "Trace.hpp"

#include <QUdpSocket>

namespace Radiant
{

  class UDPSocket::D : public QUdpSocket
  {};

  UDPSocket::UDPSocket()
  {
    m_d = new D();
  }

  UDPSocket::UDPSocket(int fd)
  {
    m_d = new D();
    if(!m_d->setSocketDescriptor(fd))
      Radiant::error("UDPSocket::UDPSocket # failed to set socket descriptor");
  }

  UDPSocket::~UDPSocket()
  {
    delete m_d;
  }
 
  int UDPSocket::open(const char * host, int port)
  {
    close();

    m_d->connectToHost(host, port);

    return 0; 
  }

  bool UDPSocket::close()
  {
    m_d->close();

    return true;
  }

  bool UDPSocket::isOpen() const
  {
    return m_d->isValid();
  }

  int UDPSocket::read(void * buffer, int bytes)
  {
    return m_d->read((char *)buffer, bytes); 
  }

  int UDPSocket::write(const void * buffer, int bytes)
  {
    return m_d->write((const char *)buffer, bytes);
  }
 
}

