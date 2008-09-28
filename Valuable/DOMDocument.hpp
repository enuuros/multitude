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

#include <vector>

namespace xercesc_2_8 {
  class DOMDocument;
  class XMLFormatTarget;
}

namespace Valuable
{
  class DOMElement;

  /// Wrapper for xercesc::DOMDocument
  class DOMDocument
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

    private:
      DOMDocument(xercesc_2_8::DOMDocument * doc);
  
      xercesc_2_8::DOMDocument * m_xDoc;

      friend bool writeDom(DOMDocument *, xercesc_2_8::XMLFormatTarget &);
  };

}

#endif
