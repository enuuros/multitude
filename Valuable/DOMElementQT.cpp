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

#include <QDomElement>

#include <Radiant/Trace.hpp>

#include <stdio.h>

#include <assert.h>

namespace Valuable
{
  struct DOMElement::Wrapped
  {
    QDomElement * x;
  };

  inline QDomElement * QELEM(DOMElement::Wrapped * x) 
  { return reinterpret_cast <QDomElement *> (x); }
  inline DOMElement::Wrapped * ELEM(QDomElement * x)
  { return reinterpret_cast <DOMElement::Wrapped *> (x); }
  
  DOMElement::DOMElement(DOMElement::Wrapped * x)
  : m_wrapped(x)
  {}
  
  std::string DOMElement::getTagName() const
  {
    if(!m_wrapped)
      return std::string();
    
    return QELEM(m_wrapped)->tagName().toStdString();
  }

  void DOMElement::appendChild(DOMElement element)
  {
    QELEM(m_wrapped)->appendChild(*QELEM(element.m_wrapped));
  }

  void DOMElement::setAttribute(const char * name, const char * value)
  {
    if(!value || !name || !m_wrapped)
      return;

    if(!strlen(value))
      return;

    QELEM(m_wrapped)->setAttribute(name, value);
  }

  DOMElement::NodeList DOMElement::getChildNodes() const
  {
    NodeList list; 

    if(!m_wrapped)
      return list;

    QDomNode n = QELEM(m_wrapped)->firstChild();

    while(!n.isNull()) {

      QDomElement de = n.toElement();

      if(!de.isNull()) {
	list.push_back(DOMElement(ELEM(new QDomElement(de))));
      }

      n = n.nextSibling();
    }
    
    return list;
  }
  
  DOMElement::NodeList DOMElement::selectChildNodes(const char * tagname) const
  {
    NodeList list; 

    if(!m_wrapped)
      return list;

    QDomNode n = QELEM(m_wrapped)->firstChild();

    while(!n.isNull()) {

      QDomElement de = n.toElement();

      if(!de.isNull() && de.tagName() == tagname) {
	list.push_back(DOMElement(ELEM(new QDomElement(de))));
      }

      n = n.nextSibling();
    }
    
    return list; 
  }

  DOMElement DOMElement::getChildNode(const char * tagname)
  {
    NodeList nodes = getChildNodes();
    
    for(NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      DOMElement e = *it;
      if(e.getTagName() == tagname)
	return e;
    }

    return DOMElement(0);
  }

  static void addSpace(FILE * f, int n)
  {
    for(int i = 0; i < n; i++) {
      fprintf(f, "  ");
    }
  }

  void DOMElement::dumpInfo(FILE * f, int recursion)
  {
    NodeList nodes = getChildNodes();

    addSpace(f, recursion);
    fprintf(f, "NODE <%s> (%d children, %d deep)",
	    getTagName().c_str(), (int) nodes.size(), recursion);

    std::string str = getTextContent();
    if(str.size() > 0 && str.size() < 100) {
      fprintf(f, " TEXT = \"%s\"", str.c_str());
    }

    fprintf(f, "\n");

    int i = 1;

    fflush(f);

    for(NodeList::iterator it = nodes.begin(); it != nodes.end(); it++) {
      addSpace(f, recursion);
      fprintf(f, "Child %d/%d\n", i, (int) nodes.size());
      (*it).dumpInfo(f, recursion + 1);
      i++;
    }
  }

  void DOMElement::setTextContent(const std::string & s)
  {
    if(isNull())
      return;
    
    QDomElement * qde = QELEM(m_wrapped);
    qde->appendChild(qde->ownerDocument().createTextNode(s.c_str()));
  }

  void DOMElement::setTextContent(const std::wstring & ws)
  {
    if(isNull())
      return;
    
    QString qs(QString::fromStdWString(ws));
    QDomElement * qde = QELEM(m_wrapped);
    qde->appendChild(qde->ownerDocument().createTextNode(qs));
  }

  std::string DOMElement::getTextContent() const
  {
    if(isNull())
      return std::string();

    return QELEM(m_wrapped)->text().toStdString();
  }

  std::wstring DOMElement::getTextContentW() const
  {
    if(isNull())
      return std::wstring();

    return QELEM(m_wrapped)->text().toStdWString();
  }

  bool DOMElement::hasAttribute(const char * name) const
  {
    if(isNull())
      return false;

    return QELEM(m_wrapped)->hasAttribute(name);
  }

  std::string DOMElement::getAttribute(const char * name) const
  {
    if(isNull())
      return std::string();

    return QELEM(m_wrapped)->attribute(name).toStdString();
  }

}
