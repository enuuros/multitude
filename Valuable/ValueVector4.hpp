#ifndef VALUABLE_VALUE_VECTOR4_HPP
#define VALUABLE_VALUE_VECTOR4_HPP

#include <Valuable/ValueVector.hpp>
#include <Nimble/Vector4.hpp>

#define VO_TYPE_VEC4I "vec4i"

namespace Valuable
{

  class ValueVector4i : public ValueVector<Nimble::Vector4i>
  {
    typedef ValueVector<Nimble::Vector4i> Base;

    public:
      ValueVector4i() : Base() {}
      ValueVector4i(HasValues * parent, const std::string & name, bool transit, const Nimble::Vector4i & v)
      : Base(parent, name, transit, v)
       {}

    const char * const type() const { return VO_TYPE_VEC4I; }

    bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> &)
    {
      using namespace xercesc;

      const XMLCh * content = element->getTextContent();
      char * myContent = XMLString::transcode(content);

      sscanf(myContent, "%d %d %d %d", &Base::m_value.x, &Base::m_value.y, &Base::m_value.z, &Base::m_value.w);

      XMLString::release(&myContent);

      return true;
    }
    
    std::string asString(bool * const ok = 0) const { 
      if(ok) *ok = true; 
      return  Radiant::StringUtils::stringify(Base::m_value.x) + std::string(" ") + 
              Radiant::StringUtils::stringify(Base::m_value.y) + std::string(" ") + 
              Radiant::StringUtils::stringify(Base::m_value.z) + std::string(" ") + 
              Radiant::StringUtils::stringify(Base::m_value.w);
    }
 
  };

}

#endif
