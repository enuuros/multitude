#ifndef HASVALUES_HASVALUES_IMPL_HPP
#define HASVALUES_HASVALUES_IMPL_HPP

#include <Valuable/HasValues.hpp>

#include <Radiant/Trace.hpp>

namespace Valuable
{

  template<class Type>
  bool HasValues::setValue(const std::string & name, const Type & v)
  {
    container::iterator it = m_children.find(name);
    if(it == m_children.end()) {
      Radiant::error("HasValues::setValue # property '%s' not found", name.c_str());
      return false;
    }

    ValueObject * vo = it->second;
    return vo->set(v);;
  }

}

#endif
