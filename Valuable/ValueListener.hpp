#ifndef VALUABLE_VALUE_LISTENER_HPP
#define VALUABLE_VALUE_LISTENER_HPP

#include <list>

namespace Valuable
{  
  class ValueObject;
  class ValueListeners;

  class ValueListener
  {
  public:
    
    friend class ValueListeners;

    ValueListener() {}
    virtual ~ValueListener();

    virtual void valueChanged(ValueObject *) = 0;
    virtual void valueDeleted(ValueObject *) = 0;

  private:

    typedef std::list<ValueListeners *> vlcontainer;
    typedef vlcontainer::iterator vliterator;

    vlcontainer m_listening;
  };

  class ValueListeners
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
