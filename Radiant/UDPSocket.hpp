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

      int read(void * buffer, int bytes, bool waitfordata = true);
      int write(const void * buffer, int bytes);

    private:
      class D;
      D * m_d;
  };

}

#endif
