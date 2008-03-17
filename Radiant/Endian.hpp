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

#ifndef DIVA_ENDIAN_H
#define DIVA_ENDIAN_H

#include <Radiant/Types.hpp>

#include <unistd.h>

/* This file contains code to determine the endianness of the
   operating system. If the system is big-endian, then macro
   WORDS_BIGENDIAN will be defined. Otherwise macro
   WORDS_LITTLEENDIAN will be defined. 

   This file also defines DIVA versions of a few common macros. This
   is needed to to bypass VR Juggler (or Performer ?) bugs.
*/

#if defined(__sgi)
// All SGI is currently big-endian
#define WORDS_BIGENDIAN 1
#define DIVA_ntohl(x) x
#define DIVA_htonl(x) x

#define DIVA_IRIX 1

#elif defined(__linux__) 

#define DIVA_LINUX 1

// This endian check only works on Linux (?)

#include <endian.h>
#include <netinet/in.h>


#define DIVA_ntohl(x) ntohl(x)
#define DIVA_htonl(x) htonl(x)
#define DIVA_ntohs(x) ntohs(x)
#define DIVA_htons(x) htons(x)

#if __BYTE_ORDER == __LITTLE_ENDIAN

#undef WORDS_BIGENDIAN
#define WORDS_LITTLEENDIAN 1

#else 

#define WORDS_BIGENDIAN 1

#endif   // __BYTE_ORDER == __LITTLE_ENDIAN

#elif defined(WIN32) //WIN32

#define WORDS_LITTLEENDIAN 1

#elif defined(__DARWIN_UNIX03)

// OSX

#define DIVA_OSX 1

#include <machine/endian.h>

#if __DARWIN_BYTE_ORDER == __DARWIN_LITTLE_ENDIAN

#define WORDS_LITTLEENDIAN 1

#else

#define WORDS_BIGENDIAN 1

#endif

#else

#error "PLATFORM NOT (YET) SUPPORTED"

#endif   // defined(__sgi)

namespace Radiant {

  inline short byteSwap2(short i)
  { 
    return (i << 8) + (i >> 8);
  }
  
  inline int byteSwap4(uint i)
  { 
    return ((i&0xff) << 24) + ((i&0xff00) << 8) + 
      ((i&0xff0000) >> 8) + ((i&0xff000000) >> 24);
  }
  
  inline ullong byteSwap8(ullong x)
  { 
#ifndef WIN32
    return ((x << 56) | ((x & 0xff00ll) << 40) | ((x & 0xff0000ll) << 24) |
	    ((x & 0xff000000ll) << 8) | ((x >> 8) & 0xff000000ll) |
	    ((x >> 24) & 0xff0000ll) | ((x >> 40) & 0xff00ll) | (x >> 56));
#else
    return ((x << 56) | ((x & 0xff00) << 40) | ((x & 0xff0000) << 24) |
	    ((x & 0xff000000) << 8) | ((x >> 8) & 0xff000000) |
	    ((x >> 24) & 0xff0000) | ((x >> 40) & 0xff00) | (x >> 56));
#endif
  }
  
}

#endif   // DIVA_ENDIAN_H
