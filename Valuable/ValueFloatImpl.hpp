#ifndef VALUABLE_VALUE_FLOAT_IMPL_HPP
#define VALUABLE_VALUE_FLOAT_IMPL_HPP

#include <Valuable/ValueFloat.hpp>
#include <Valuable/DOMElement.hpp>

namespace Valuable
{

  template<class T>
  bool ValueFloatT<T>::deserializeXML(DOMElement e)
  {
    Base::m_value = atof(e.getTextContent().c_str());
    return true;
  }

}

#endif
