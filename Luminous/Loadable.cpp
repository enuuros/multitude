/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/Loadable.hpp>
#include <Radiant/Trace.hpp>

namespace Luminous
{

  Loadable::Loadable(const std::string& filepath):
    m_filepath(filepath),
    m_canDelete(false),
    m_loader(0),
    m_userData(0)
  {}

  Loadable::~Loadable() 
  {}

}
