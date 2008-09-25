#ifndef VALUABLE_VALUE_FLOAT_HPP
#define VALUABLE_VALUE_FLOAT_HPP

#include <Valuable/ValueNumeric.hpp>

#include <Radiant/StringUtils.hpp>

#define STD_OP this->emitChange(); return *this;

#define VO_TYPE_FLOAT "float"

namespace Valuable
{

  template<class T>
  class ValueFloatT : public ValueNumeric<T>
  {
    typedef ValueNumeric<T> Base;

    public:
      ValueFloatT() : Base() {}
      ValueFloatT(HasValues * parent, const std::string & name, T v, bool transit = false)
      : ValueNumeric<T>(parent, name, v, transit)
      {}

      inline ValueFloatT<T> & operator = (T i) { Base::m_value = i; STD_OP }

      inline operator const T & () const { return Base::m_value; }

      const char * const type() const { return VO_TYPE_FLOAT; }

      bool deserializeXML(DOMElement element);
  };

  typedef ValueFloatT<float> ValueFloat;
}

#undef STD_OP

#endif
