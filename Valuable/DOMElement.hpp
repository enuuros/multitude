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

#ifndef VALUABLE_DOM_ELEMENT_HPP
#define VALUABLE_DOM_ELEMENT_HPP

#include <Valuable/Export.hpp>

#include <string>
#include <list>

namespace Valuable
{
  /// Utility namespace for handling the xerces_2_x namespaces in Valuable
  /** We use namespace mxercesc, as xerces is just a macro around the
     xercesc_2_7 or xercesc_2_8 namespaces. */
  namespace mxercesc {
    class DOMElement;
  }


  /// Wrapper for xercesc::DOMElement
  class VALUABLE_API DOMElement
  {
    public:
      DOMElement(mxercesc::DOMElement * xElement);
      ~DOMElement() {}

      bool null() const { return (m_xElement == 0);  }

      std::string getTagName() const;

      void appendChild(DOMElement element);
      void setAttribute(const char * name, const char * value);

      bool hasAttribute(const char * name) const;
      std::string getAttribute(const char * name) const;

      std::string getTextContent() const;
      std::wstring getTextContentW() const;

      void setTextContent(const std::string & content);
      void setTextContent(const std::wstring & content);

      typedef std::list<DOMElement> NodeList;
      NodeList getChildNodes() const;

    private:
       mxercesc::DOMElement * m_xElement;

      friend class DOMDocument;
  };

}

#endif
