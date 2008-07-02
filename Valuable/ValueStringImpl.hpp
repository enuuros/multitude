#ifndef VALUABLE_VALUE_STRING_IMPL_HPP
#define VALUABLE_VALUE_STRING_IMPL_HPP

#include "ValueString.hpp"

namespace Valuable
{

  template<class T>
  bool ValueStringT<T>::deserializeXML(xercesc::DOMElement * element)
  {
    using namespace xercesc;

    const XMLCh * content = element->getTextContent();
    char * myContent = XMLString::transcode(content);

    m_value = T(myContent);

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
  bool ValueStringT<T>::set(const std::string & v) 
  { 
    m_value = T(v); 
    return true; 
  }
 
}

#endif
