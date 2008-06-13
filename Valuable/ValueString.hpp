#ifndef VALUABLE_VALUE_STRING_HPP
#define VALUABLE_VALUE_STRING_HPP

#include <Valuable/ValueNumeric.hpp>

#include <Radiant/StringUtils.hpp>

#define STD_OP this->emitChange(); return *this;

#define VO_TYPE_STRING "string"

namespace Valuable
{

  template<class T>
  class ValueStringT : public ValueObject
  {
    public:
      ValueStringT() : ValueObject() {}
      ValueStringT(HasValues * parent, const std::string & name, bool transit, T v)
      : ValueObject(parent, name, transit),
      m_value(v)
      {}

      ValueStringT<T> & operator = (const ValueStringT<T> & i) { m_value = i.m_value; STD_OP }
      ValueStringT<T> & operator = (const T & i) { m_value = i; STD_OP }

      float       asFloat(bool * const ok = 0) const   { if(ok) *ok = true; return atof(m_value.c_str()); }
      int         asInt(bool * const ok = 0) const     { if(ok) *ok = true; return atoi(m_value.c_str()); }
      std::string asString(bool * const ok = 0) const  { if(ok) *ok = true; return m_value; }
 
      const char * const type() const { return VO_TYPE_STRING; }

      bool deserializeXML(xercesc::DOMElement * element)
      {
        using namespace xercesc;

        const XMLCh * content = element->getTextContent();
        char * myContent = XMLString::transcode(content);

        m_value = std::string(myContent);

        XMLString::release(&myContent);

        return true;
      }

    private:
      T m_value;
  };

  typedef ValueStringT<std::string> ValueString;  
}

#undef STD_OP

#endif
