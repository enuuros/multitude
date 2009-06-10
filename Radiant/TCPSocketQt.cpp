#include "TCPSocket.hpp"
#include "Trace.hpp"

#include <QTcpSocket>

namespace Radiant
{

  class TCPSocket::D : public QTcpSocket
  {};

  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  TCPSocket::TCPSocket()
  {
    m_d = new D();
  }

  TCPSocket::TCPSocket(int fd)
  {
    m_d = new D();
    if(!m_d->setSocketDescriptor(fd))
      Radiant::error("TCPSocket::TCPSocket # setSocketDescriptor failed");
  }

  TCPSocket::~TCPSocket()
  {
    delete m_d;
  }

  int TCPSocket::open(const char * host, int port)
  {
    close();

    m_d->connectToHost(host, port);

    return 0;
  }

  bool TCPSocket::close()
  {
    m_d->close();

    return true;
  }
 
  bool TCPSocket::isOpen() const
  {
    return m_d->isValid();
  }

  const char * TCPSocket::host() const
  {
    return m_d->peerName().toAscii();
  }

  int TCPSocket::port() const
  {
    return m_d->peerPort();
  }

  int TCPSocket::read(void * buffer, int bytes)
  {
    return m_d->read((char *)buffer, bytes);
  }

  int TCPSocket::write(const void * buffer, int bytes)
  {
    return m_d->write((const char *)buffer, bytes);
  }

  bool TCPSocket::isHungUp() const
  {
    return (m_d->state() != QAbstractSocket::ConnectedState);
//    return m_d->q->isValid();
  }

  bool TCPSocket::isPendingInput(unsigned int waitMicroSeconds)
  {
    return m_d->waitForReadyRead(waitMicroSeconds);
  }

  void TCPSocket::debug()
  {
    Radiant::info("TCPSocket::debug #");
    Radiant::info("\tSTATE %d", m_d->state());
    Radiant::info("\tVALID %d", m_d->isValid());
    Radiant::info("\tERROR %d", m_d->error());
  }

}
