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

#ifndef VALUABLE_VALUE_COLOR_HPP
#define VALUABLE_VALUE_COLOR_HPP

#include <Radiant/Color.hpp>

#include <Valuable/Export.hpp>
#include <Valuable/ValueObject.hpp>

#define VALUEMIT_STD_OP this->emitChange(); return *this;

namespace Valuable
{

  class VALUABLE_API ValueColor : public ValueObject
  {
  public:
    ValueColor(HasValues * parent, const std::string & name, const Radiant::Color & c, bool transit = false);
    
    ValueColor & operator = (const Radiant::Color & c) { m_color = c; VALUEMIT_STD_OP }
    
    const char * type() const { return "color"; }
    
    std::string asString(bool * const ok = 0) const;
    
    bool deserializeXML(DOMElement element);
    
    Radiant::Color asColor() const { return m_color; }
    
    virtual bool set(const Nimble::Vector4f & v);
    
    inline float red() const   { return m_color[0]; }
    inline float green() const { return m_color[1]; }
    inline float blue() const  { return m_color[2]; }
    inline float alpha() const { return m_color[3]; }

  protected:
    Radiant::Color m_color;
  };

}

#undef VALUEMIT_STD_OP

#endif
