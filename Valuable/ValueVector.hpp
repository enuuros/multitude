#ifndef VALUABLE_VALUE_VECTOR_HPP
#define VALUABLE_VALUE_VECTOR_HPP

#include <Valuable/ValueObject.hpp>

#include <Nimble/Vector4.hpp>

#include <Radiant/StringUtils.hpp>

#include <sstream>

#define STD_OP emitChange(); return *this;

namespace Valuable
{

  template<class VectorType, typename ElementType, int N>
  class ValueVector : public ValueObject
  {
    public:
      ValueVector() : ValueObject() {}
      ValueVector(HasValues * parent, const std::string & name, const VectorType & v, bool transit = false) : ValueObject(parent, name, transit), m_value(v) {}

      ValueVector<VectorType, ElementType, N> & operator =
      (const VectorType & v) { m_value = v; emitChange(); return *this; }

      ValueVector<VectorType, ElementType, N> & operator += 
      (const VectorType & v) { m_value += v; emitChange(); return *this; }
      ValueVector<VectorType, ElementType, N> & operator -= 
      (const VectorType & v) { m_value -= v; emitChange(); return *this; }

    VectorType operator - 
      (const VectorType & v) const { return m_value - v; }
    VectorType operator + 
      (const VectorType & v) const { return m_value + v; }

      ElementType operator [] (int i) const { return m_value[i]; }      

      virtual bool deserializeXML(DOMElement element);

      const char * const type() const;

      virtual bool set(const VectorType & v);

      const VectorType & asVector() const { return m_value; }
      const VectorType & operator * () const { return m_value; }

      std::string asString(bool * const ok = 0) const;

    protected:
      VectorType m_value;
  };

  typedef ValueVector<Nimble::Vector2i, int, 2> ValueVector2i;
  typedef ValueVector<Nimble::Vector3i, int, 3> ValueVector3i;
  typedef ValueVector<Nimble::Vector4i, int, 4> ValueVector4i;

  typedef ValueVector<Nimble::Vector2f, float, 2> ValueVector2f;
  typedef ValueVector<Nimble::Vector3f, float, 3> ValueVector3f;
  typedef ValueVector<Nimble::Vector4f, float, 4> ValueVector4f;

}


#undef STD_OP

#endif
