#ifndef VALUABLE_VALUE_STRING_IMPL_HPP
#define VALUABLE_VALUE_STRING_IMPL_HPP

#include "ValueString.hpp"

#include <Radiant/StringUtils.hpp>

#define STD_EM this->emitChange();

namespace Valuable
{

  template<class T>
  bool ValueStringT<T>::deserializeXML(xercesc::DOMElement * element)
  {
    using namespace xercesc;

    const XMLCh * content = element->getTextContent();
    char * myContent = XMLString::transcode(content);

    m_value = T(myContent);

    STD_EM;

    XMLString::release(&myContent);

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
  bool ValueStringT<std::wstring>::deserializeXML(xercesc::DOMElement * element)
  {
    using namespace xercesc;

    const XMLCh * content = element->getTextContent();
    int len = XMLString::stringLen(content); 

    m_value.resize(len);

    for(int i = 0; i < len; i++) {
      m_value[i] = len;
    }

    STD_EM;

    return true;
  }
 
}

#endif
