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

#include "ValueColor.hpp"
#include "DOMElement.hpp"

#include <Radiant/StringUtils.hpp>

namespace Valuable
{

  ValueColor::ValueColor(HasValues * parent, const std::string & name, const Radiant::Color & c, bool transit)
    : ValueObject(parent, name, transit),
    m_color(c)
  {}

  bool ValueColor::deserializeXML(DOMElement element) {
    std::stringstream in(element.getTextContent());

    for(int i = 0; i < 4; i++)
      in >> m_color[i];

    return true;
  }

  std::string ValueColor::asString(bool * const ok) const {
    if(ok) *ok = true;

    std::string r = Radiant::StringUtils::stringify(m_color[0]);

    for(int i = 1; i < 4; i++)
      r += std::string(" ") + Radiant::StringUtils::stringify(m_color[i]);

    return r;
  }

  bool ValueColor::set(const Nimble::Vector4f & v)
  {
    m_color = v;
    return true;
  }

}

