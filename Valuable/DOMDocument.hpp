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