/* COPYRIGHT
 *
 * This file is part of ValueIO.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ValueIO.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUEIO_DOM_STORE_HPP
#define VALUEIO_DOM_STORE_HPP

#include <ValueIO/Dom.hpp>

#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

namespace ValueIO {

  class DOMStore
  {
    public:
      DOMStore() : m_doc(0), m_rootElem(0), m_parser(0) {}
      ~DOMStore() {}

      DOMImplementation * implementation()
      {
        static const XMLCh impltype[] = { chLatin_L, chLatin_S, chNull };
        return DOMImplementationRegistry::getDOMImplementation(impltype);
      }

      DOMElement * prepareSave(const char * filename, const char * docname)
      {

        try {
          m_filename = filename;

          DOMImplementation* impl = implementation();

          m_doc = impl->createDocument(0, XW(docname), 0);

          m_rootElem = m_doc->getDocumentElement();

          return m_rootElem;
        }
        catch (...) {
        }

        return 0;
      }

      bool finishSave()
      {
        bool ok = false;

        static const XMLCh UTF8[] = 
        { chLatin_U, chLatin_T, chLatin_F, chDigit_8, chNull };

        try {
          // m_rootElem->appendChild(writeDom(m_doc));

          DOMImplementation* impl = implementation();
          DOMWriter* serializer = 
            ((DOMImplementationLS*) impl)->createDOMWriter();

          serializer->setEncoding(UTF8);
          serializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
          LocalFileFormatTarget target(m_filename.c_str());

          ok = serializer->writeNode( & target, *m_doc);

          serializer->release();
        }
        catch(...) {
        }
        return ok;
      }

      bool prepareLoad(const char * filename)
      {
        bool ok = false;

        try {

          DOMImplementation *impl = implementation();

          m_parser = ((DOMImplementationLS*)impl)->createDOMBuilder
            (DOMImplementationLS::MODE_SYNCHRONOUS, 0);

          m_parser->resetDocumentPool();

          m_doc = m_parser->parseURI(filename);

          if(!m_doc) {
            puts("DomStore::prepareLoad # parsing failed");
            ok = false;
          }
          else
            ok = true;
        }
        catch(...) {
          puts("DomStore::prepareLoad # DOM trouble");
        }

        return ok;
      }

      bool finishLoad()
      {
        if(m_parser) {
          m_parser->release();
          m_parser = 0;
        }
        return true;
      }

      DOMDocument * doc() { return m_doc; }

    protected:
      DOMDocument * m_doc;
      DOMElement  * m_rootElem;
      DOMBuilder  * m_parser;
      std::string  m_filename;
  };


}

#endif
