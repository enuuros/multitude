/* COPYRIGHT
 *
 * This file is part of Valuable.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Valuable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUABLE_VALUE_LISTENER_HPP
#define VALUABLE_VALUE_LISTENER_HPP

#include <Valuable/Export.hpp>

#include <list>

namespace Valuable
{  
  class ValueObject;
  class ValueListeners;

  class VALUABLE_API ValueListener
  {
  public:
    
    friend class ValueListeners;

    ValueListener() {}
    virtual ~ValueListener();

    virtual void valueChanged(ValueObject *) = 0;
    virtual void valueDeleted(ValueObject *) = 0;

  private:

    void removeObject(ValueListeners *);


    typedef std::list<ValueListeners *> vlcontainer;
    typedef vlcontainer::iterator vliterator;

    vlcontainer m_listening;
  };

  class VALUABLE_API ValueListeners
  {
  public:
    typedef std::list<ValueListener *> container;
    typedef container::iterator iterator;

    ValueListeners() : m_list(0) {}
    ValueListeners(const ValueListeners & that);

    ~ValueListeners();

    void push_back(ValueListener * listener);
    void remove(ValueListener * listener);

    unsigned size() const { return m_list ? m_list->size(): 0; }

    iterator begin() { return m_list->begin(); }
    iterator end() { return m_list->end(); }

    void emitChange(ValueObject *);
    void emitDelete(ValueObject *);

    ValueListeners & operator = (const ValueListeners & that);
  private:

    void makeList()
    {
      if(!m_list)
	m_list = new container;
    }

    container * m_list;
  };

}

#endif
