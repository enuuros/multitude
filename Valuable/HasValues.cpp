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

namespace Valuable
{
  HasValues::HasValues()
  : ValueObject()
  {}

  HasValues::HasValues(HasValues * parent, const std::string & name, bool transit)
  : ValueObject(parent, name, transit)
  {
  }

  HasValues::~HasValues()
  {
//    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
//      delete it->second;
//    }
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
      Radiant::trace(Radiant::ERROR, "HasValues::addValue # can not add child '%s' as '%s' "
                     "already has a child with the same name.",
                     cname.c_str(), m_name.c_str());
      return false;
    }

    // Unlink parent if necessary
    HasValues * parent = value->parent();
    if(parent) {
      Radiant::trace(Radiant::ERROR, "HasValues::addValue # '%s' already has a parent '%s'. "
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
      Radiant::trace(Radiant::ERROR, "HasValues::removeValue # '%s' is not a child of '%s'.", 
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
    if(e.null()) {
      Radiant::trace(Radiant::ERROR, "HasValues::saveToFileXML # object failed to serialize");
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
      Radiant::trace(Radiant::ERROR, "HasValues::serializeXML # attempt to serialize object with no name");
      return DOMElement(0);
    }

    DOMElement elem = doc->createElement(m_name.c_str());
    if(elem.null()) {
      Radiant::trace(Radiant::ERROR, "HasValues::serializeXML # failed to create XML element");
      return DOMElement(0);
    }

    elem.setAttribute("type", type());
    
    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
      ValueObject * vo = it->second;
  
      DOMElement child = vo->serializeXML(doc);
      if(!child.null()) 
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
        Radiant::trace(Radiant::ERROR, "HasValues::deserializeXML # (%s) don't know how to handle element '%s'", type(), name.c_str());
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

  bool HasValues::readElement(DOMElement )
  {
    return false;
  }

  // Template functions must be instantiated to be exported
  template VALUABLE_API bool HasValues::setValue<float>(const std::string & name, const float &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector2T<float> >(const std::string & name, const Nimble::Vector2T<float> &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector4T<float> >(const std::string & name, const Nimble::Vector4T<float> &);


}
