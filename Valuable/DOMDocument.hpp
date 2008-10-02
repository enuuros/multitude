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

#ifndef VALUABLE_DOM_DOCUMENT_HPP
#define VALUABLE_DOM_DOCUMENT_HPP

#include <Radiant/RefPtr.hpp>

#include <Valuable/Export.hpp>

#include <vector>

namespace Valuable
{

  namespace mxercesc {
    class DOMDocument;
    class XMLFormatTarget;
  }

  class DOMElement;

  /// Wrapper for xercesc::DOMDocument
  class VALUABLE_API DOMDocument
  {
  public:

      ~DOMDocument();

      static DOMDocument * createDocument();

      DOMElement createElement(const char * name);

      void appendChild(DOMElement element);

      bool writeToFile(const char * filename);
      bool writeToMem(std::vector<char> & buf);

      bool readFromFile(const char * filename);

      DOMElement getDocumentElement();

    mxercesc::DOMDocument * document() { return m_xDoc; }

    private:
      DOMDocument(mxercesc::DOMDocument * doc);
  
    mxercesc::DOMDocument * m_xDoc;

    friend bool writeDom(DOMDocument *, mxercesc::XMLFormatTarget &);
  };

}

#endif
