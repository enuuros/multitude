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

#include "DOMElement.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

#include <Radiant/Trace.hpp>

using namespace xercesc;

namespace Valuable
{

  DOMElement::DOMElement(xercesc::DOMElement * xElement)
  : m_xElement(xElement)
  {}
  
  std::string DOMElement::getTagName() const
  {
    char * name = XMLString::transcode(m_xElement->getTagName());
    std::string str(name);
    XMLString::release(&name);

    return str;    
  }

  void DOMElement::appendChild(DOMElement element)
  {
    m_xElement->appendChild(element.m_xElement);
  }

  void DOMElement::setAttribute(const char * name, const char * value)
  {
    XMLCh * xName = XMLString::transcode(name);
    XMLCh * xValue = XMLString::transcode(value);

    m_xElement->setAttribute(xName, xValue);

    XMLString::release(&xName);
    XMLString::release(&xValue);
  }

  DOMElement::NodeList DOMElement::getChildNodes() const
  {
    NodeList list; 

    DOMNodeList * xList = m_xElement->getChildNodes();

    for(XMLSize_t i = 0; i < xList->getLength(); i++) {
      xercesc::DOMElement * xe = dynamic_cast<xercesc::DOMElement *> (xList->item(i));
      if(!xe) continue;

      list.push_back(DOMElement(xe));    
    }
    
    return list;
  }

  void DOMElement::setTextContent(const std::string & s)
  {
    XMLCh * xCont = XMLString::transcode(s.c_str());
    m_xElement->setTextContent(xCont);
    XMLString::release(&xCont);
  }

  void DOMElement::setTextContent(const std::wstring & ws)
  {
    std::basic_string<XMLCh> xs(ws.begin(), ws.end());

    m_xElement->setTextContent(xs.c_str());
  }

  std::string DOMElement::getTextContent() const
  {
    const XMLCh * xContent = m_xElement->getTextContent();
    char * content = XMLString::transcode(xContent);

    std::string result(content);
    XMLString::release(&content);

    return result;
  }

  std::wstring DOMElement::getTextContentW() const
  {
    const XMLCh * xContent = m_xElement->getTextContent();
    size_t len = XMLString::stringLen(xContent);
      
    std::wstring result;
    result.resize(len);

    for(size_t i = 0; i < len; i++)
      result[i] = xContent[i];
   
    return result;    
  }

  bool DOMElement::hasAttribute(const char * name) const
  {
    XMLCh * xName = XMLString::transcode(name);
    bool r = m_xElement->hasAttribute(xName);
    XMLString::release(&xName);

    return r;
  }

  std::string DOMElement::getAttribute(const char * name) const
  {
    XMLCh * xName = XMLString::transcode(name);

    const XMLCh * xValue = m_xElement->getAttribute(xName);
    char * value = XMLString::transcode(xValue);

    std::string r(value);

    XMLString::release(&value);
    XMLString::release(&xName);

    return r;
  }

}
