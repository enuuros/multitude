#ifndef VALUABLE_VALUE_INT_IMPL_HPP
#define VALUABLE_VALUE_INT_IMPL_HPP

#include <Valuable/ValueInt.hpp>
#include <Valuable/DOMElement.hpp>

#include <Radiant/StringUtils.hpp>

namespace Valuable
{

  template<class T>
  bool ValueIntT<T>::deserializeXML(DOMElement e)
  {
    Base::m_value = Radiant::StringUtils::fromString<T>(e.getTextContent().c_str());
    return true;
  }

  template<>
  bool ValueIntT<Radiant::TimeStamp>::deserializeXML(DOMElement )
  {
    Radiant::error("ValueIntT<Radiant::TimeStamp>::deserializeXML # not implemented!");
    return false;
  }

}

#endif
