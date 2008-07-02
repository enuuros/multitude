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
      ValueStringT(HasValues * parent, const std::string & name, T v, bool transit = false)
      : ValueObject(parent, name, transit),
      m_value(v)
      {}

      ValueStringT<T> & operator = (const ValueStringT<T> & i) { m_value = i.m_value; STD_OP }
      ValueStringT<T> & operator = (const T & i) { m_value = i; STD_OP }

      float asFloat(bool * const ok = 0) const;
      int asInt(bool * const ok = 0) const;
      std::string asString(bool * const ok = 0) const;

      virtual bool set(const std::string & v);
 
      const char * const type() const { return VO_TYPE_STRING; }

      bool deserializeXML(xercesc::DOMElement * element);

    private:
      T m_value;
  };

  typedef ValueStringT<std::string> ValueString;  
  typedef ValueStringT<std::wstring> ValueWString;  
}

#undef STD_OP

#endif
