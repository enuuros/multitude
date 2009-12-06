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

#include "DOMDocument.hpp"
#include "DOMElement.hpp"

#include <Radiant/FileUtils.hpp>
#include <Radiant/Trace.hpp>

#include <QDomDocument>
#include <QFile>

#include <assert.h>

namespace Valuable
{
  struct DOMDocument::Wrapped {
    QDomDocument * x;
  };

  inline QDomDocument * QDOC(DOMDocument::Wrapped * x) { return reinterpret_cast<QDomDocument *> (x); }
  inline QDomElement * QELEM(DOMElement::Wrapped * x) { return reinterpret_cast<QDomElement *> (x); }

  inline DOMDocument::Wrapped * DOC(QDomDocument * x) { return reinterpret_cast<DOMDocument::Wrapped *> (x); }
  inline DOMElement::Wrapped * ELEM(QDomElement * x) { return reinterpret_cast<DOMElement::Wrapped *> (x); }

  DOMDocument::DOMDocument(DOMDocument::Wrapped * doc)
    : m_wrapped(doc)
  {}

  DOMDocument::~DOMDocument()
  {
    if(m_wrapped)
      delete QDOC(m_wrapped);
  }

  void DOMDocument::appendChild(DOMElement element)
  {
    if(element.isNull()) return;

    assert(m_wrapped != 0);
    
    QDOC(m_wrapped)->documentElement().appendChild(*QELEM(element.m_wrapped));
  }

  DOMDocument * DOMDocument::createDocument()
  {
    return new DOMDocument(DOC(new QDomDocument));
  }

  DOMElement DOMDocument::createElement(const char * name)
  {
    QDomElement de = QDOC(m_wrapped)->createElement(name);
    return DOMElement(ELEM(new QDomElement(de)));
  }

  DOMElement DOMDocument::createElement(const std::string & name)
  {
    return createElement(name.c_str());
  }


  bool DOMDocument::writeToFile(const char * filename)
  {
    return Radiant::FileUtils::writeTextFile
      (filename, QDOC(m_wrapped)->toByteArray().data());
  }

  bool DOMDocument::writeToMem(std::vector<char> & buffer)
  {
    QDomDocument * qdoc = QDOC(m_wrapped);
    std::string xml = qdoc->toString().toStdString();

    buffer.resize(xml.size());

    memcpy( & buffer[0], xml.c_str(), xml.size());
    return true;
  }

  bool DOMDocument::readFromFile(const char * filename, bool validate)
  {
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
      return false;

    if (!QDOC(m_wrapped)->setContent( & file)) {
      file.close();
      return false;
    }

    file.close();
    return true;
  }
   
  DOMElement DOMDocument::getDocumentElement()
  {
    QDomElement * de = new QDomElement(QDOC(m_wrapped)->documentElement());
    return DOMElement(ELEM(de));
  }

}
