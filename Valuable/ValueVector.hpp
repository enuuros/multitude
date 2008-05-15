#ifndef VALUABLE_VALUE_VECTOR_HPP
#define VALUABLE_VALUE_VECTOR_HPP

#include <Valuable/ValueObject.hpp>

#define STD_OP emitChange(); return *this;

namespace Valuable
{

  template<class T>
  class ValueVector : public ValueObject
  {
    public:
      ValueVector() : ValueObject() {}
      ValueVector(HasValues * parent, const std::string & name, bool transit, const T & v) : ValueObject(parent, name, transit), m_value(v) {}

      ValueVector<T> & operator = (const ValueVector<T> & v) { m_value = v.m_value; STD_OP }
      ValueVector<T> & operator = (const T & v) { m_value = v; STD_OP }

      ValueVector<T> & operator -= (const T & v) { m_value -= v; STD_OP }
      ValueVector<T> & operator += (const T & v) { m_value -= v; STD_OP }      

    protected:
      T m_value;
  };

}

#undef STD_OP

#endif
