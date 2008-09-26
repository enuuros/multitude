#include "DOMDocument.hpp"
#include "DOMElement.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

#include <Radiant/Trace.hpp>

using namespace xercesc;

namespace Valuable
{
  DOMDocument::DOMDocument(xercesc::DOMDocument * doc)
    : m_xDoc(doc)
  {}

  DOMDocument::~DOMDocument()
  {
    if(m_xDoc)
      m_xDoc->release();
  }

  void DOMDocument::appendChild(DOMElement element)
  {
    if(element.null()) return;

    assert(m_xDoc != 0);

    m_xDoc->appendChild(element.m_xElement);
  }

  DOMDocument * DOMDocument::createDocument()
  {
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a document & writer
    xercesc::DOMDocument * doc = impl->createDocument();
    return new DOMDocument(doc);
  }

  DOMElement DOMDocument::createElement(const char * name)
  {
    XMLCh * xName = XMLString::transcode(name);
    xercesc::DOMElement * elem = 0;

    try {
      elem = m_xDoc->createElement(xName);
    } catch(const DOMException & e) {
      char * msg = XMLString::transcode(e.getMessage());
      Radiant::error("DOMDocument::createElement # %s", msg);
      XMLString::release(&msg);
    }

    XMLString::release(&xName);

    return DOMElement(elem);
  }

  bool writeDom(DOMDocument * doc, XMLFormatTarget & target)
  {
     // Get implementation of the Load-Store (LS) interface
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a document & writer
    DOMWriter * writer = ((DOMImplementationLS*)impl)->createDOMWriter();

    bool result = true;

    try {
      // Output pretty XML
      writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
      writer->writeNode(&target, *doc->m_xDoc);

      // Flush the target just to be sure all contents are written
      target.flush();
    } catch(const XMLException & e)  {
      char * msg = XMLString::transcode(e.getMessage());
      Radiant::error("DOMDocument::save # %s", msg);
      XMLString::release(&msg);
      result = false;
    } catch(const DOMException & e) {
      char * msg = XMLString::transcode(e.msg);
      Radiant::error("DOMDocument::save # %s", msg);
      XMLString::release(&msg);
      result = false;
    }

    // Cleanup
    writer->release();

    return result;
  }

  bool DOMDocument::writeToFile(const char * filename)
  {
    try {
      LocalFileFormatTarget target(filename);
      return writeDom(this, target);
    } catch(...) {
      return false;
    }
  }

  bool DOMDocument::writeToMem(std::vector<char> & buffer)
  {
    MemBufFormatTarget target;
    if(!writeDom(this, target)) {
      buffer.clear();
      return false;
    }

    size_t bytes = target.getLen();
    buffer.resize(bytes);

    memcpy(&buffer[0], target.getRawBuffer(), bytes);
    return true;
  }

  bool DOMDocument::readFromFile(const char * filename)
  {
    // Get implementation of the Load-Store (LS) interface
    const XMLCh LS[] = {chLatin_L, chLatin_S, chNull};
    DOMImplementation * impl = DOMImplementationRegistry::getDOMImplementation(LS);

    // Create a parser
    DOMBuilder * parser = ((DOMImplementationLS*)impl)->createDOMBuilder
      (DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    xercesc::DOMDocument * parsed = parser->parseURI(filename);
   
    // Clone the document because the parsed
    if(m_xDoc)
      m_xDoc->release();

    if(parsed)
      m_xDoc = (xercesc::DOMDocument *)parsed->cloneNode(parsed);
    else
      m_xDoc = 0;

    parser->release();

    return (m_xDoc != 0);
  }
   
  DOMElement DOMDocument::getDocumentElement()
  {
    xercesc::DOMElement * de = m_xDoc->getDocumentElement();
    return DOMElement(de);
  }

}
