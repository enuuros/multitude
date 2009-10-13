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

#include "ValueObject.hpp"
#include "HasValues.hpp"
#include "ChangeMap.hpp"

#include "DOMElement.hpp"
#include "DOMDocument.hpp"

#include <Radiant/Trace.hpp>

#include <algorithm>

namespace Valuable
{
  using namespace Radiant;

  ValueObject::ValueObject()
  : m_parent(0),
    m_transit(false),
    m_eventsEnabled(true)
  {}

  ValueObject::ValueObject(HasValues * parent, const std::string & name, bool transit)
    : m_parent(0),
      m_name(name),
      m_transit(transit),
      m_eventsEnabled(true)
  {
    if(parent)
      parent->addValue(name, this);
  }

  ValueObject::ValueObject(const ValueObject & o)
    : m_parent(0)
  {
    m_name = o.m_name;
    m_transit = o.m_transit;
    m_eventsEnabled = o.m_eventsEnabled;
  }

  ValueObject::~ValueObject()
  {
    emitDelete();

    for(Sources::iterator it = m_eventSources.begin(); it != m_eventSources.end(); it++) {
      (*it)->eventRemoveListener(this);
    }

    for(Listeners::iterator it = m_elisteners.begin();
        it != m_elisteners.end(); it++) {
      (*it).m_listener->eventRemoveSource(this);
    }


  }

  std::string ValueObject::path() const
  {
    if(m_parent) 
      return m_parent->path() + "/" + m_name;
    
    return "/" + m_name;
  }
  
  void ValueObject::processMessage(const char *, Radiant::BinaryData & )
  {

  }

  float ValueObject::asFloat(bool * ok) const
  {   
    if(ok) *ok = false;
    Radiant::error(
"ValueObject::asFloat # %s : conversion not available", m_name.c_str());
    return 0.0f;
  }

  int ValueObject::asInt(bool * ok) const
  {
    if(ok) *ok = false;
    Radiant::error(
"ValueObject::asInt # %s : conversion not available", m_name.c_str());
    return 0;
  }

  std::string ValueObject::asString(bool * ok) const
  {
    if(ok) *ok = false;
    Radiant::error(
"ValueObject::asString # %s : conversion not available", m_name.c_str());
    return "";
  }

  DOMElement ValueObject::serializeXML(DOMDocument * doc) 
  {
    if(m_name.empty()) {
      Radiant::error(
"ValueObject::serializeXML # attempt to serialize object with no name");
      return DOMElement(0);
    }

    DOMElement elem = doc->createElement(m_name.c_str());
    elem.setAttribute("type", type());
    elem.setTextContent(asString());

    return elem;   
  }

  void ValueObject::eventAddListener(const char * from,
                                     const char * to,
                                     Valuable::ValueObject * obj)
  {
    ValuePass vp;
    vp.m_listener = obj;
    vp.m_from = from;
    vp.m_to = to;

    if(std::find(m_elisteners.begin(), m_elisteners.end(), vp) != 
       m_elisteners.end())
      debug("Widget::eventAddListener # Already got item %s -> %s (%p)",
	    from, to, obj);
    else {
      m_elisteners.push_back(vp);
      obj->eventAddSource(this);
    }
  }
  
  void ValueObject::eventRemoveListener(Valuable::ValueObject * obj)
  {
    for(Listeners::iterator it = m_elisteners.begin(); it != m_elisteners.end();){
      if((*it).m_listener == obj) {
	it = m_elisteners.erase(it);
      }
      else
	it++;
    }
  }

  void ValueObject::eventAddSource(Valuable::ValueObject * source)
  {
    m_eventSources.insert(source);
  }

  void ValueObject::eventRemoveSource(Valuable::ValueObject * source)
  {
    Sources::iterator it = m_eventSources.find(source);

    if(it != m_eventSources.end())
      m_eventSources.erase(it);
  }


  void ValueObject::eventSend(const std::string & id, Radiant::BinaryData & bd)
  {
    eventSend(id.c_str(), bd);
  }

  void ValueObject::eventSend(const char * id, Radiant::BinaryData & bd)
  {
    if(!id || !m_eventsEnabled)
      return;

    bd.rewind();

    for(Listeners::iterator it = m_elisteners.begin(); it != m_elisteners.end(); it++) {
      ValuePass & vp = *it;
      if(vp.m_from == id)
        vp.m_listener->processMessage(vp.m_to.c_str(), bd);
    }
  }

  void ValueObject::eventSend(const char * id)
  {
    Radiant::BinaryData tmp;
    eventSend(id, tmp);
  }

  void ValueObject::emitChange()
  {
//    Radiant::trace("ValueObject::emitChange # '%s'", m_name.c_str());
    m_listeners.emitChange(this);
    ChangeMap::addChange(this);
  }

  void ValueObject::emitDelete()
  {
    //Radiant::trace("ValueObject::emitDelete");
    m_listeners.emitDelete(this);
    ChangeMap::addDelete(this);
  }

  void ValueObject::removeParent()
  {
    if(m_parent) {
      m_parent->removeValue(this);
      m_parent = 0;
    }
  }

  bool ValueObject::set(float )
  {
    Radiant::error(
"ValueObject::set(float) # conversion not available");
    return false;
  }

  bool ValueObject::set(int )
  {
    Radiant::error(
"ValueObject::set(int) # conversion not available");
    return false;
  }

  bool ValueObject::set(const std::string & )
  {
    Radiant::error(
"ValueObject::set(string) # conversion not available");
    return false;
  }

  bool ValueObject::set(const Nimble::Vector2f & )
  {
    Radiant::error(
"ValueObject::set(Vector2f) # conversion not available");
    return false;
  }

  bool ValueObject::set(const Nimble::Vector4f & )
  {
    Radiant::error(
"ValueObject::set(Vector4f) # conversion not available");
    return false;
  }

}
