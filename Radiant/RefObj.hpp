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

#ifndef RADIANT_REF_OBJ_HPP
#define RADIANT_REF_OBJ_HPP

#include <Radiant/Export.hpp>

namespace Radiant {
  
  /// Utility for RefObj
  template <typename T>
  class RADIANT_API RefObjInt
  {
  public:
    RefObjInt() : m_count(1) {}
    
    T m_object;
    unsigned m_count;
  };
  
  /// Smart object reference
  /** This class can be used to share an object between several
      holders, using reference counters. */
  template <typename T>
  class RADIANT_API RefObj
  {
  public:
    RefObj() : m_holder(0) {}
    /// Creates a RefObj that copies the argument object
    RefObj(const T &obj) 
      : m_holder(new RefObjInt<T>) { m_holder->m_object = obj; }
    /// Share the object pointer with another RefObj
    RefObj(const RefObj &that)
    {
      m_holder = ((RefObj *) & that)->m_holder; 
      if(m_holder)
	m_holder->m_count++;
    }
  
    /// Deletes the RefObj, potentially deleting the object
    ~RefObj() { breakLink(); }

    /// Get a reference to the object
    T & ref() { check(); return m_holder->m_object; }
    /// Get a constant reference to the object
    const T & ref() const { check(); return m_holder->m_object; }

    /// An operator to get a reference to the object
    T & operator *() { check(); return m_holder->m_object; }
    /// An operator to get a constant reference to the object
    const T & operator *() const { check(); return m_holder->m_object; }

    /// Create a deep copy of the object
    /** Instead of sharing a link to an object, this method creates a
	real copy of the object. */
    void deepCopy(const RefObj &that)
    { 
      if(!m_holder)
	m_holder = new RefObjInt<T>;
      else if(m_holder->count > 1) {
	--m_holder->m_count;
	m_holder = new RefObjInt<T>;
      }    
      m_holder->m_object = that.m_holder->m_object;
    }

    /// Link to the source object
    RefObj & operator = (const RefObj &that)
    {
      that.check();
      if(m_holder == that.m_holder) return *this;

      breakLink(); 
      m_holder = ((RefObj *) & that)->m_holder; 
      m_holder->m_count++;

      return *this;
    }

  private:
    /// Breaks the link to the object.
    void breakLink()
    { 
      if(!m_holder) return; 
      if(! --m_holder->m_count) 
	delete m_holder; 
      m_holder = 0;
    }

    /// Makes the internal object exist, if it did not exist earlier.
    void check() const
    { 
      if(!m_holder)
	((RefObj *) this)->m_holder = new RefObjInt<T>; 
    }
  
    RefObjInt<T> *m_holder;
  };


}

#endif
