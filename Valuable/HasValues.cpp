#include <Valuable/HasValues.hpp>

#include <Radiant/Trace.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

using namespace xercesc;

namespace Valuable
{

  HasValues::HasValues(HasValues * parent, const std::string & name, bool transit)
  : ValueObject(parent, name, transit)
  {
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
      Radiant::error("HasValues::addValue # can not add child '%s' as '%s' already has a child with the same name.", cname.c_str(), m_name.c_str());
      return false;
    }

    // Unlink parent if necessary
    HasValues * parent = value->parent();
    if(parent) {
      Radiant::error("HasValues::addValue # '%s' already has a parent '%s'. Unlinking it to set new parent.", cname.c_str(), parent->name().c_str());
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
      Radiant::error("HasValues::removeValue # '%s' is not a child of '%s'.", cname.c_str(), m_name.c_str());
      return;
    }

    m_children.erase(it);
    value->m_parent = 0;
  }

  bool HasValues::saveXML(const char * filename)
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

      LocalFileFormatTarget target(filename);

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

  bool HasValues::loadXML(const char * filename, CL::ClassLoader<ValueObject> & cl)
  {
    // Get implementation of the Load-Store (LS) interface
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a parser
    DOMBuilder * parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    DOMDocument * doc = parser->parseURI(filename);
    if(!doc) return false;

    bool r = deserializeXML(doc->getDocumentElement(), cl);

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

  bool HasValues::deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & cl)
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
        vo->deserializeXML(ce, cl);
      else if(!readElement(ce, cl)) {
        Radiant::error("HasValues::deserializeXML # don't know how to handle element '%s'", nameVal);
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

  bool HasValues::readElement(xercesc::DOMElement * , CL::ClassLoader<ValueObject> & )
  {
/*
    char * nameVal = XMLString::transcode(ce->getTagName());
    XMLCh * typeAttr = XMLString::transcode("type");

    // Get the 'type' attribute
    if(!ce->hasAttribute(typeAttr)) {
      Radiant::error("HasValues::readElement # no type attribute on element '%s'", nameVal);
      XMLString::release(&nameVal);
      XMLString::release(&typeAttr);
      return false;
    }

    const XMLCh * typeVal = ce->getAttribute(typeAttr);

    // Instantiate from type
    char * myType = XMLString::transcode(typeVal);
    ValueObject * vo = cl.instantiate(myType);
    XMLString::release(&myType);

    // Add as child & recurse
    addValue(nameVal, vo);
    vo->deserializeXML(ce, cl);

    XMLString::release(&nameVal);
    XMLString::release(&typeAttr);

    return true;
*/
    return false;
  }

}
