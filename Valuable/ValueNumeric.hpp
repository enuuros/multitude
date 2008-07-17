#ifndef VALUABLE_VALUE_NUMERIC_HPP
#define VALUABLE_VALUE_NUMERIC_HPP

#include <Valuable/ValueObject.hpp>

#include <Radiant/StringUtils.hpp>

#define STD_OP emitChange(); return *this;

namespace Valuable
{

  template<class T>
  class ValueNumeric : public ValueObject
  {
    public:
      ValueNumeric() : ValueObject(), m_value(T(0)) {}
      ValueNumeric(HasValues * parent, const std::string & name, T v, bool transit = false)
      : ValueObject(parent, name, transit),
      m_value(v)
      {}

      ValueNumeric<T> & operator = (const ValueNumeric<T> & vi) { m_value = vi.m_value; STD_OP }
      ValueNumeric<T> & operator = (T i) { m_value = i;  STD_OP }

      ValueNumeric<T> & operator -= (T i) { m_value -= i; STD_OP }
      ValueNumeric<T> & operator += (T i) { m_value += i; STD_OP }
      ValueNumeric<T> & operator *= (T i) { m_value *= i; STD_OP }
      ValueNumeric<T> & operator /= (T i) { m_value /= i; STD_OP }

      float asFloat(bool * const ok = 0) const { if(ok) *ok = true; return static_cast<float> (m_value); }
      int asInt(bool * const ok = 0) const { if(ok) *ok = true; return static_cast<int> (m_value); }
      std::string asString(bool * const ok = 0) const { if(ok) *ok = true; return Radiant::StringUtils::stringify(m_value); }

      virtual bool set(int v) { m_value = static_cast<T> (v); return true; }
      virtual bool set(float v) { m_value = static_cast<T> (v); return true; }

    protected:
      T m_value;
  };

}

#undef STD_OP

#endif
