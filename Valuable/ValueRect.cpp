#include "ValueRect.hpp"
#include "DOMElement.hpp"

#include <Radiant/StringUtils.hpp>

namespace Valuable
{

  ValueRect::ValueRect(HasValues * parent, const std::string & name, const Nimble::Rect & r, bool transit)
    : ValueObject(parent, name, transit),
    m_rect(r)
  {}

  bool ValueRect::deserializeXML(DOMElement element) {
    std::stringstream in(element.getTextContent());

    Nimble::Vector2f lo, hi;

    in >> lo[0]; 
    in >> lo[1]; 
    in >> hi[0]; 
    in >> hi[1]; 

    m_rect.setLow(lo);
    m_rect.setHigh(hi);

    return true;
  }

  std::string ValueRect::asString(bool * const ok) const {
    if(ok) *ok = true;

    const Nimble::Vector2f & lo = m_rect.low();
    const Nimble::Vector2f & hi = m_rect.high();

  
    
    std::string r = Radiant::StringUtils::stringify(lo[0]);
    r += std::string(" ") + Radiant::StringUtils::stringify(lo[1]);
    r += std::string(" ") + Radiant::StringUtils::stringify(hi[0]);
    r += std::string(" ") + Radiant::StringUtils::stringify(hi[1]);

    return r;
  }
/*
  bool ValueRect::set(const Nimble::Vector4f & v)
  {
    m_color = v;
    return true;
  }
*/
}

