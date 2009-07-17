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

