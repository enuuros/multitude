/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */



#ifndef DIVA_BINARY_STREAM_HPP
#define DIVA_BINARY_STREAM_HPP



namespace Radiant {

  /// Abstract base class for binary streams
  class BinaryStream
  {
  public:
    BinaryStream() {}
    virtual ~BinaryStream() {}

    /// Read bytes from the stream
    virtual int read(void * buffer, int bytes) = 0;
    /// Write bytes to the stream
    virtual int write(const void * buffer, int bytes) = 0;
    
  };

}


#endif
