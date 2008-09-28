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

#ifndef LUMINOUS_COLLECTABLE_HPP
#define LUMINOUS_COLLECTABLE_HPP

#include <Radiant/Export.hpp>

namespace Luminous
{
  class GarbageCollector;

  /// A utility class to make the work of GarbageCollector easier.
  class MTEXPORT Collectable
  {
  public:
    Collectable(GarbageCollector * collector = 0);
    virtual ~Collectable();

    GarbageCollector * gc() { return m_collector; }

  private:
    GarbageCollector * m_collector;
  };

}

#endif
