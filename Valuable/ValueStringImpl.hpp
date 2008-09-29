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

#ifndef VALUABLE_VALUE_STRING_IMPL_HPP
#define VALUABLE_VALUE_STRING_IMPL_HPP

#include "ValueString.hpp"
#include "DOMElement.hpp"
#include "DOMDocument.hpp"

#include <Radiant/StringUtils.hpp>

#define STD_EM this->emitChange();

namespace Valuable
{

  template<class T>
  bool ValueStringT<T>::deserializeXML(DOMElement element)
  {
    m_value = T(element.getTextContent());

    STD_EM;

    return true;
  }

  template<class T>
  float ValueStringT<T>::asFloat(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    return atof(m_value.c_str()); 
  }

  template<class T>
  int ValueStringT<T>::asInt(bool * const ok) const 
  {
    if(ok) *ok = true; 
    return atoi(m_value.c_str());
  }

  template<class T>
  std::string ValueStringT<T>::asString(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    return m_value; 
  }

  template<class T>
  std::wstring ValueStringT<T>::asWString(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    std::wstring tmp;
    Radiant::StringUtils::utf8ToStdWstring(tmp, m_value);
    return tmp; 
  }

  template<class T>
  bool ValueStringT<T>::set(const std::string & v) 
  { 
    m_value = T(v); 
    STD_EM;
    return true; 
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  // Specializations for wide strings.

  template <>
  float ValueStringT<std::wstring>::asFloat(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    std::string tmp;
    Radiant::StringUtils::stdWstringToUtf8(tmp, m_value);
    return atof(tmp.c_str()); 
  }

  template <>
  int ValueStringT<std::wstring>::asInt(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    std::string tmp;
    Radiant::StringUtils::stdWstringToUtf8(tmp, m_value);
    return atoi(tmp.c_str()); 
  }

  template<>
  std::string ValueStringT<std::wstring>::asString(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    std::string tmp;
    Radiant::StringUtils::stdWstringToUtf8(tmp, m_value);
    return tmp; 
  }

  template<>
  std::wstring ValueStringT<std::wstring>::asWString(bool * const ok) const 
  { 
    if(ok) *ok = true; 
    return m_value;
  }

  template<>
  bool ValueStringT<std::wstring>::set(const std::string & v) 
  { 
    Radiant::StringUtils::utf8ToStdWstring(m_value, v); 
    STD_EM;
    return true; 
  }

  template<>
  DOMElement ValueStringT<std::string>::serializeXML(DOMDocument * doc)
  {
    return ValueObject::serializeXML(doc);
  }

  template<>
  DOMElement ValueStringT<std::wstring>::serializeXML(DOMDocument * doc)
  {
    if(m_name.empty()) {
      Radiant::error("ValueWString::serializeXML # attempt to serialize object with no name");
      return DOMElement(0);
    }

    DOMElement elem = doc->createElement(m_name.c_str());
    elem.setAttribute("type", type());
  
    const std::wstring & ws = asWString();
    elem.setTextContent(ws);

    return elem;
  }

  template<>
  bool ValueStringT<std::wstring>::deserializeXML(DOMElement element)
  {
    m_value = element.getTextContentW();
    STD_EM;
    return true;
  }
 
}

#endif
