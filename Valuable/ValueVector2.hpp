#ifndef VALUABLE_VALUE_VECTOR2_HPP
#define VALUABLE_VALUE_VECTOR2_HPP

#include <Valuable/ValueVector.hpp>

#define VO_TYPE_VEC2I "vec2i"

namespace Valuable
{

  class ValueVector2i : public ValueVector<Nimble::Vector2i>
  {
    typedef ValueVector<Nimble::Vector2i> Base;

    public:
      ValueVector2i() : Base() {}
      ValueVector2i(HasValues * parent, const std::string & name, bool transit, const Nimble::Vector2i & v)
      : Base(parent, name, transit, v)
       {}

    const char * const type() const { return VO_TYPE_VEC2I; }

    bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> &)
    {
      using namespace xercesc;

      const XMLCh * content = element->getTextContent();
      char * myContent = XMLString::transcode(content);

      sscanf(myContent, "%d %d", &Base::m_value.x, &Base::m_value.y);

      XMLString::release(&myContent);

      return true;
    }
    
    std::string asString(bool * const ok = 0) const { 
      if(ok) *ok = true; 
      return  Radiant::StringUtils::stringify(Base::m_value.x) + std::string(" ") + 
              Radiant::StringUtils::stringify(Base::m_value.y);
    }
 
  };

}

#endif
