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

#ifndef VALUABLE_VALUE_FLOAT_HPP
#define VALUABLE_VALUE_FLOAT_HPP

#include <Radiant/StringUtils.hpp>

#include <Valuable/Export.hpp>
#include <Valuable/ValueNumeric.hpp>

#define STD_OP this->emitChange(); return *this;

#define VO_TYPE_FLOAT "float"

namespace Valuable
{

  template<class T>
  class VALUABLE_API ValueFloatT : public ValueNumeric<T>
  {
    typedef ValueNumeric<T> Base;

    public:
      ValueFloatT() : Base() {}
      ValueFloatT(HasValues * parent, const std::string & name, T v, bool transit = false)
      : ValueNumeric<T>(parent, name, v, transit)
      {}

      inline ValueFloatT<T> & operator = (T i) { Base::m_value = i; STD_OP }

      inline operator const T & () const { return Base::m_value; }

      const char * const type() const { return VO_TYPE_FLOAT; }

      bool deserializeXML(DOMElement element);
  };

  typedef ValueFloatT<float> ValueFloat;

#ifdef WIN32
#ifdef VALUABLE_EXPORT
  // In WIN32 template classes must be instantiated to be exported
  template class ValueFloatT<float>;
#endif
#endif

}

#undef STD_OP

#endif
