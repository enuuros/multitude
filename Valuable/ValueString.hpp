/* COPYRIGHT
 *
 * This file is part of Valuable.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Valuable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUABLE_VALUE_STRING_HPP
#define VALUABLE_VALUE_STRING_HPP

#include <Radiant/StringUtils.hpp>

#include <Valuable/Export.hpp>
#include <Valuable/ValueNumeric.hpp>

#define STD_OP this->emitChange(); return *this;

#define VO_TYPE_STRING "string"

namespace Valuable
{

  template<class T>
  class VALUABLE_API ValueStringT : public ValueObject
  {
    public:
      ValueStringT() : ValueObject() {}
      ValueStringT(HasValues * parent, const std::string & name, T v, bool transit = false)
      : ValueObject(parent, name, transit),
      m_value(v)
      {}

      ValueStringT<T> & operator = (const ValueStringT<T> & i) { m_value = i.m_value; STD_OP }
      ValueStringT<T> & operator = (const T & i) { m_value = i; STD_OP }

      float asFloat(bool * const ok = 0) const;
      int asInt(bool * const ok = 0) const;
      std::string asString(bool * const ok = 0) const;
      std::wstring asWString(bool * const ok = 0) const;

      virtual bool set(const std::string & v);
 
      const char * const type() const { return VO_TYPE_STRING; }

      DOMElement serializeXML(DOMDocument * doc);
      bool deserializeXML(DOMElement element);      

    private:
      T m_value;
  };

  typedef ValueStringT<std::string> ValueString;  
  typedef ValueStringT<std::wstring> ValueWString;

  // Instantiation of template classes
  // See ValueStringImpl.hpp for std::wstring member specializations

}

#undef STD_OP

#endif
