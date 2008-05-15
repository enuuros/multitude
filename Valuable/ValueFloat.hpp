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
      ValueFloatT(HasValues * parent, const std::string & name, bool transit, T v)
      : ValueNumeric<T>(parent, name, transit, v)
      {}

      ValueFloatT<T> & operator = (T i) { Base::m_value = i; STD_OP }

      const char * const type() const { return VO_TYPE_FLOAT; }

      bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> &) {
        using namespace xercesc;

        const XMLCh * content = element->getTextContent();
        char * myContent = XMLString::transcode(content);

        Base::m_value = atof(myContent);

        XMLString::release(&myContent);
  
        return true;
      }
  };

}

#undef STD_OP

#endif
