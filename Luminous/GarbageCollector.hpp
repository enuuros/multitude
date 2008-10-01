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

#ifndef LUMINOUS_GARBAGE_COLLECTOR_HPP
#define LUMINOUS_GARBAGE_COLLECTOR_HPP

#include <Luminous/Export.hpp>

#include <set>

namespace Luminous
{
  
  /// This class is used to keep track of objects that have been deleted.

  /** The objects are represented by simple void pointers. This should
      be enough to find the objects from stogare tables, in particular
      from GLResources. */
  class LUMINOUS_API GarbageCollector
  {
  public:

    typedef std::set<void *> container;
    typedef container::iterator iterator;

    GarbageCollector();
    ~GarbageCollector();

    /// Empties the garbage list.
    void clear();

    /// Adds the obj to the list of deleted objects
    void objectDeleted(void * obj);

    /// Iterator to the beginning of the list
    iterator begin() { return m_items.begin(); }
    /// Iterator to the end of the list
    iterator end() { return m_items.end(); }

    /// Gets an object from an interator
    /** By using this method to access the object, you can guarantee
      that your code will work even if the container type is
      changed. */
    static void * getObject(iterator & it) { return (*it); }

  protected:
    container m_items;
  };
}

#endif
