#include "ValueObject.hpp"
#include "HasValues.hpp"
#include "ChangeMap.hpp"

#include <Radiant/Trace.hpp>

namespace Valuable
{

  ValueObject::ValueObject(HasValues * parent, const std::string & name, bool transit)
    : m_parent(0),
    m_name(name),
    m_transit(transit)
  {
    if(parent)
      parent->addValue(name, this);
  }

  ValueObject::~ValueObject()
  {
    emitDelete();
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
    Radiant::error("ValueObject::asFloat # %s : conversion not available", m_name.c_str());
    return 0.0f;
  }

  int ValueObject::asInt(bool * ok) const
  {
    if(ok) *ok = false;
    Radiant::error("ValueObject::asInt # %s : conversion not available", m_name.c_str());
    return 0;
  }

  std::string ValueObject::asString(bool * ok) const
  {
    if(ok) *ok = false;
    Radiant::error("ValueObject::asString # %s : conversion not available", m_name.c_str());
    return "";
  }

  xercesc::DOMElement * ValueObject::serializeXML(xercesc::DOMDocument * doc) 
  {
    using namespace xercesc;

    XMLCh * name = XMLString::transcode(m_name.c_str());
    XMLCh * value = XMLString::transcode(asString().c_str());
    XMLCh * typeAttr = XMLString::transcode("type");
    XMLCh * typeVal = XMLString::transcode(type());

    DOMElement * elem = doc->createElement(name);
    elem->setAttribute(typeAttr, typeVal);    
    elem->setTextContent(value);

    XMLString::release(&name);
    XMLString::release(&value);
    XMLString::release(&typeAttr);
    XMLString::release(&typeVal);

    return elem;   
  }

  void ValueObject::emitChange()
  {
    Radiant::trace("ValueObject::emitChange");
    ChangeMap::addChange(this);
  }

  void ValueObject::emitDelete()
  {
    Radiant::trace("ValueObject::emitDelete");
    ChangeMap::addDelete(this);
  }

  void ValueObject::removeParent()
  {
    if(m_parent) {
      m_parent->removeValue(this);
      m_parent = 0;
    }
  }

}
