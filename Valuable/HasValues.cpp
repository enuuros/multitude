//#include <Valuable/HasValues.hpp>
#include <Valuable/HasValuesImpl.hpp>

#include <Radiant/Trace.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

using namespace xercesc;

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
      Radiant::error("HasValues::addValue # can not add child '%s' as '%s' "
                     "already has a child with the same name.",
                     cname.c_str(), m_name.c_str());
      return false;
    }

    // Unlink parent if necessary
    HasValues * parent = value->parent();
    if(parent) {
      Radiant::error("HasValues::addValue # '%s' already has a parent '%s'. "
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
      Radiant::error("HasValues::removeValue # '%s' is not a child of '%s'.", 
                     cname.c_str(), m_name.c_str());
      return;
    }

    m_children.erase(it);
    value->m_parent = 0;
  }
  
  bool HasValues::saveXML(XMLFormatTarget & target)
  {
    // Get implementation of the Load-Store (LS) interface
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a document & writer
    DOMDocument * doc = impl->createDocument();
    DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();

    try {
      // Fill the document
      doc->appendChild(serializeXML(doc));

      // Output pretty XML
      writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      writer->writeNode(&target, *doc);

      // Flush the target just to be sure all contents are written
      target.flush();
    } catch(const XMLException & e)  {
      char * msg = XMLString::transcode(e.getMessage());
      Radiant::error("HasValues::saveXML # %s", msg);
      XMLString::release(&msg);
    } catch(const DOMException & e) {
      char * msg = XMLString::transcode(e.msg);
      Radiant::error("HasValues::saveXML # %s", msg);
      XMLString::release(&msg);
    }

    // Cleanup
    writer->release();
    doc->release();

    return true;
  }

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

  xercesc::DOMElement * HasValues::serializeXML(DOMDocument * doc)
  {
    XMLCh * name = XMLString::transcode(m_name.c_str());
    XMLCh * typeAttr = XMLString::transcode("type");
    XMLCh * typeVal = XMLString::transcode(type());

    DOMElement * elem = doc->createElement(name);
    elem->setAttribute(typeAttr, typeVal);    

    XMLString::release(&name);
    XMLString::release(&typeAttr);
    XMLString::release(&typeVal);

    // Write children
    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
      ValueObject * vo = it->second;

      DOMElement * child = vo->serializeXML(doc);
      if(child) 
        elem->appendChild(child);
    }

    return elem;
  }

  bool HasValues::deserializeXML(xercesc::DOMElement * element)
  {
    // Name
    char * nameVal = XMLString::transcode(element->getTagName());
    m_name = nameVal;
    XMLString::release(&nameVal);

    // Children
    DOMNodeList * list = element->getChildNodes();

    for(XMLSize_t i = 0; i < list->getLength(); i++) {
      DOMElement * ce = dynamic_cast<DOMElement *> (list->item(i));
      if(!ce) continue;

      // Get the value matching the element tag
      nameVal = XMLString::transcode(ce->getTagName());

//      Radiant::trace("DEBUG: tag %s", nameVal);

      ValueObject * vo = getValue(nameVal);

      // If the value exists, just deserialize it. Otherwise, pass the element
      // to readElement()
      if(vo)
        vo->deserializeXML(ce);
      else if(!readElement(ce)) {
        Radiant::error("HasValues::deserializeXML # don't know how to handle element '%s'",
                       nameVal);
        XMLString::release(&nameVal);
        return false;
      }

      XMLString::release(&nameVal);
    }

    return true;
  }

  void HasValues::debugDump() {
    Radiant::trace("%s {", m_name.c_str());

    for(container::iterator it = m_children.begin(); it != m_children.end(); it++) {
      ValueObject * vo = it->second;

      HasValues * hv = dynamic_cast<HasValues *> (vo);
      if(hv) hv->debugDump();
      else {
        std::string s = vo->asString();
        Radiant::trace("\t%s = %s", vo->name().c_str(), s.c_str());
      }
    }

    Radiant::trace("}");
  }

  bool HasValues::readElement(xercesc::DOMElement *)
  {
    return false;
  }

  template bool HasValues::setValue<Nimble::Vector2T<float> >(const std::string & name, const Nimble::Vector2T<float> &);
  template bool HasValues::setValue<Nimble::Vector4T<float> >(const std::string & name, const Nimble::Vector4T<float> &);

}
