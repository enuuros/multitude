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

#include <Valuable/HasValuesImpl.hpp>
#include <Valuable/DOMDocument.hpp>
#include <Valuable/DOMElement.hpp>

#include <Valuable/HasValues.hpp>

#include <Radiant/Trace.hpp>
#include <Radiant/RefPtr.hpp>

#include <algorithm>


namespace Valuable
{
  using namespace Radiant;

  HasValues::HasValues()
  : ValueObject(),
    m_eventsEnabled(true)
  {}

  HasValues::HasValues(HasValues * parent, const std::string & name, bool transit)
  : ValueObject(parent, name, transit),
    m_eventsEnabled(true)
  {
  }

  HasValues::~HasValues()
  {
    for(Sources::iterator it = m_eventSources.begin(); it != m_eventSources.end(); it++) {
      (*it)->eventRemoveListener(this);
    }

    for(Listeners::iterator it = m_elisteners.begin();
        it != m_elisteners.end(); it++) {
      (*it).m_listener->eventRemoveSource(this);
    }

  }

  ValueObject * HasValues::getValue(const std::string & name)
  {
    container::iterator it = m_children.find(name);

    return it == m_children.end() ? 0 : it->second;
  }

  bool HasValues::addValue(const std::string & cname, ValueObject * const value)
  {
//    Radiant::trace("HasValues::addValue # adding %s", cname.c_str());

    // Check children
    if(m_children.find(cname) != m_children.end()) {
      Radiant::error(
"HasValues::addValue # can not add child '%s' as '%s' "
                     "already has a child with the same name.",
                     cname.c_str(), m_name.c_str());
      return false;
    }

    // Unlink parent if necessary
    HasValues * parent = value->parent();
    if(parent) {
      Radiant::error(
"HasValues::addValue # '%s' already has a parent '%s'. "
                     "Unlinking it to set new parent.",
                     cname.c_str(), parent->name().c_str());
      value->removeParent();  
    }
  
    // Change the value name
    value->setName(cname);

    m_children[value->name()] = value;
    value->m_parent  = this;

    return true;
  }

  void HasValues::removeValue(ValueObject * const value)
  {
    const std::string & cname = value->name();
  
    container::iterator it = m_children.find(cname);
    if(it == m_children.end()) {
      Radiant::error(
"HasValues::removeValue # '%s' is not a child of '%s'.", 
                     cname.c_str(), m_name.c_str());
      return;
    }

    m_children.erase(it);
    value->m_parent = 0;
  }

  bool HasValues::saveToFileXML(const char * filename)
  {
    Radiant::RefPtr<DOMDocument> doc = DOMDocument::createDocument();
    DOMElement e = serializeXML(doc.ptr());
    if(e.isNull()) {
      Radiant::error(
"HasValues::saveToFileXML # object failed to serialize");
      return false;
    }

    doc->appendChild(e);

    return doc->writeToFile(filename);
  }

  bool HasValues::saveToMemoryXML(std::vector<char> & buffer)
  {
    Radiant::RefPtr<DOMDocument> doc = DOMDocument::createDocument();
    doc->appendChild(serializeXML(doc.ptr()));

    return doc->writeToMem(buffer);
  }  

  bool HasValues::loadFromFileXML(const char * filename)
  {
    Radiant::RefPtr<DOMDocument> doc = DOMDocument::createDocument();
    
    if(!doc->readFromFile(filename)) 
      return false;

    DOMElement e = doc->getDocumentElement();
    return deserializeXML(e);
  }

/*  
  bool HasValues::saveXML(XMLFormatTarget & target)
  {
    Radiant::RefPtr<DOMDocument> doc = DOMDocument::createDocument();
    doc->appendChild(serializeXML(doc));

    return doc->save();
  }

#ifdef WIN32
#pragma warning (disable:4702)
#endif

  bool HasValues::saveXML(const char * filename)
  {
    try {
      LocalFileFormatTarget target(filename);
      return saveXML(target);
    }
    catch(...) {
      return false;
    }

	  return false; // Unreachable
  }

#ifdef WIN32
#pragma warning (default:4702)
#endif

  bool HasValues::saveInMemoryXML(std::vector<char> & buffer)
  {
    MemBufFormatTarget target;
    if(!saveXML(target)) {
      buffer.clear();
      return false;
    }

    unsigned size = target.getLen();

    buffer.resize(size);
    memcpy(& buffer[0], target.getRawBuffer(), size);

    return true;
  }

  bool HasValues::loadXML(const char * filename)
  {
    // Get implementation of the Load-Store (LS) interface
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a parser
    DOMBuilder * parser = ((DOMImplementationLS*)impl)->createDOMBuilder
      (DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    DOMDocument * doc = parser->parseURI(filename);
    if(!doc) return false;

    bool r = deserializeXML(doc->getDocumentElement());

    parser->release();

    return r;
  }
*/
  DOMElement HasValues::serializeXML(DOMDocument * doc)
  {
    if(m_name.empty()) {
      Radiant::error(
"HasValues::serializeXML # attempt to serialize object with no name");
      return DOMElement(0);
    }

    DOMElement elem = doc->createElement(m_name.c_str());
    if(elem.isNull()) {
      Radiant::error(
"HasValues::serializeXML # failed to create XML element");
      return DOMElement(0);
    }

    elem.setAttribute("type", type());
    
    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
      ValueObject * vo = it->second;
  
      DOMElement child = vo->serializeXML(doc);
      if(!child.isNull()) 
        elem.appendChild(child);
    }

    return elem;
  }

  bool HasValues::deserializeXML(DOMElement element)
  {
    // Name
    m_name = element.getTagName();

    // Children
    DOMElement::NodeList list = element.getChildNodes();

    for(DOMElement::NodeList::iterator it = list.begin(); it != list.end(); it++) {
      const DOMElement & elem = *it;

      std::string name = elem.getTagName();

      ValueObject * vo = getValue(name);
      
      // If the value exists, just deserialize it. Otherwise, pass the element
      // to readElement()
      if(vo)
        vo->deserializeXML(elem);
      else if(!readElement(elem)) {
        Radiant::error(
"HasValues::deserializeXML # (%s) don't know how to handle element '%s'", type(), name.c_str());
        return false;
      }
    }

    return true;
  }

  void HasValues::debugDump() {
    Radiant::trace(Radiant::DEBUG, "%s {", m_name.c_str());

    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
      ValueObject * vo = it->second;

      HasValues * hv = dynamic_cast<HasValues *> (vo);
      if(hv) hv->debugDump();
      else {
        std::string s = vo->asString();
        Radiant::trace(Radiant::DEBUG, "\t%s = %s", vo->name().c_str(), s.c_str());
      }
    }

    Radiant::trace(Radiant::DEBUG, "}");
  }

  void HasValues::eventAddListener(const char * from,
                                   const char * to,
                                   Valuable::HasValues * obj,
                                   const Radiant::BinaryData * defaultData)
  {
    ValuePass vp;
    vp.m_listener = obj;
    vp.m_from = from;
    vp.m_to = to;

    if(defaultData)
      vp.m_defaultData = *defaultData;

    if(std::find(m_elisteners.begin(), m_elisteners.end(), vp) != 
       m_elisteners.end())
      debug("Widget::eventAddListener # Already got item %s -> %s (%p)",
	    from, to, obj);
    else {
      m_elisteners.push_back(vp);
      obj->eventAddSource(this);
    }
  }
    void eventAddListener(const char * from,
                          const char * to,
                          Valuable::HasValues * obj,
                          const Radiant::BinaryData & defaultData );
  
  void HasValues::eventRemoveListener(Valuable::HasValues * obj)
  {
    for(Listeners::iterator it = m_elisteners.begin(); it != m_elisteners.end();){
      if((*it).m_listener == obj) {
	it = m_elisteners.erase(it);
      }
      else
	it++;
    }
  }

  void HasValues::eventAddSource(Valuable::HasValues * source)
  {
    m_eventSources.insert(source);
  }

  void HasValues::eventRemoveSource(Valuable::HasValues * source)
  {
    Sources::iterator it = m_eventSources.find(source);

    if(it != m_eventSources.end())
      m_eventSources.erase(it);
  }

  void HasValues::processMessage(const char * id, Radiant::BinaryData & data)
  {
    if(!id)
      return;

    const char * delim = strchr(id, '/');
    
    std::string key(id);
    int skip;

    if(delim) {
      skip = delim - id;
      key.erase(key.begin() + skip, key.end());
    }
    else
      skip = key.size();

    ValueObject * vo = getValue(id);

    if(vo) {
      info("HasValues::processMessage # Sending message \"%s\" to %s",
	   id + skip, typeid(*vo).name());
      vo->processMessage(id + skip, data);
    }
  }


  void HasValues::eventSend(const std::string & id, Radiant::BinaryData & bd)
  {
    eventSend(id.c_str(), bd);
  }

  void HasValues::eventSend(const char * id, Radiant::BinaryData & bd)
  {
    if(!id || !m_eventsEnabled)
      return;

    for(Listeners::iterator it = m_elisteners.begin(); it != m_elisteners.end(); it++) {
      ValuePass & vp = *it;
      if(vp.m_from == id) {

        BinaryData & bdsend = vp.m_defaultData.total() ? vp.m_defaultData : bd;

        bdsend.rewind();

        vp.m_listener->processMessage(vp.m_to.c_str(), bdsend);
        
      }
    }
  }

  void HasValues::eventSend(const char * id)
  {
    Radiant::BinaryData tmp;
    eventSend(id, tmp);
  }


  bool HasValues::readElement(DOMElement )
  {
    return false;
  }

  // Template functions must be instantiated to be exported
  template VALUABLE_API bool HasValues::setValue<float>(const std::string & name, const float &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector2T<float> >(const std::string & name, const Nimble::Vector2T<float> &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector4T<float> >(const std::string & name, const Nimble::Vector4T<float> &);


}
