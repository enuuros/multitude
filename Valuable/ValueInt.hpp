#ifndef VALUABLE_VALUE_INT_HPP
#define VALUABLE_VALUE_INT_HPP

#include <Valuable/ValueNumeric.hpp>

#include <Radiant/StringUtils.hpp>

#define STD_OP this->emitChange(); return *this;

#define VO_TYPE_INT "int"

namespace Valuable
{

  template<class T>
  class ValueIntT : public ValueNumeric<T>
  {
    typedef ValueNumeric<T> Base;

    public:
      ValueIntT() : Base() {}
      ValueIntT(HasValues * parent, const std::string & name, bool transit, T v)
      : ValueNumeric<T>(parent, name, transit, v)
      {}

      ValueIntT<T> & operator = (T i) { Base::m_value = i; STD_OP }

      // Postfix
      ValueIntT<T> & operator ++ (int) { Base::m_value++; STD_OP }
      ValueIntT<T> & operator -- (int) { Base::m_value--; STD_OP }

      // Prefix
      ValueIntT<T> & operator ++ () { ++Base::m_value; STD_OP }
      ValueIntT<T> & operator -- () { --Base::m_value; STD_OP }

      // Shift
      ValueIntT<T> & operator <<= (int i) { Base::m_value <<= i; STD_OP }
      ValueIntT<T> & operator >>= (int i) { Base::m_value >>= i; STD_OP }

      const char * const type() const { return VO_TYPE_INT; }

      bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & ) {
        using namespace xercesc;

        const XMLCh * content = element->getTextContent();
        char * myContent = XMLString::transcode(content);

        Base::m_value = atoi(myContent);

        XMLString::release(&myContent);

        return true;
      }
  };

}

#undef STD_OP

#endif
