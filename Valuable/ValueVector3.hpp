#ifndef VALUABLE_VALUE_VECTOR3_HPP
#define VALUABLE_VALUE_VECTOR3_HPP

#include <Valuable/ValueVector.hpp>
#include <Nimble/Vector3.hpp>

#define VO_TYPE_VEC3I "vec3i"

namespace Valuable
{

  class ValueVector3i : public ValueVector<Nimble::Vector3i>
  {
    typedef ValueVector<Nimble::Vector3i> Base;

    public:
      ValueVector3i() : Base() {}
      ValueVector3i(HasValues * parent, const std::string & name, bool transit, const Nimble::Vector3i & v)
      : Base(parent, name, transit, v)
       {}

    const char * const type() const { return VO_TYPE_VEC3I; }

    bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> &)
    {
      using namespace xercesc;

      const XMLCh * content = element->getTextContent();
      char * myContent = XMLString::transcode(content);

      sscanf(myContent, "%d %d %d", &Base::m_value.x, &Base::m_value.y, &Base::m_value.z);

      XMLString::release(&myContent);

      return true;
    }
    
    std::string asString(bool * const ok = 0) const { 
      if(ok) *ok = true; 
      return  Radiant::StringUtils::stringify(Base::m_value.x) + std::string(" ") + 
              Radiant::StringUtils::stringify(Base::m_value.y) + std::string(" ") +
              Radiant::StringUtils::stringify(Base::m_value.z);
    }
 
  };

}

#endif
