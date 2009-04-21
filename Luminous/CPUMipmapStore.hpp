/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef LUMINOUS_CPU_MIPMAP_STORE_HPP
#define LUMINOUS_CPU_MIPMAP_STORE_HPP

#include <Luminous/CPUMipmaps.hpp>

namespace Luminous {

  class GarbageCollector;

  class LUMINOUS_API CPUMipmapStore
  {
  public:
    
    static CPUMipmaps * acquire(const std::string & filename, GarbageCollector * gc = 0);
    static CPUMipmaps * acquire(const char * filename, GarbageCollector * gc = 0);

    static void release(CPUMipmaps *);
  };
}

#endif
