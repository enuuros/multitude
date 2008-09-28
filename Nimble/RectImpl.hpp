/* COPYRIGHT
 *
 * This file is part of ConfigReader.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "ConfigReader.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Nimble/Rect.hpp>

namespace Nimble
{

  template <class T> 
    bool RectT<T>::contains(Vector2T<T> v) const
    {
      return ((v[0] >= m_low[0]) && (v[0] <= m_high[0]) && (v[1] >= m_low[1]) && (v[1] <= m_high[1]));  
    }


}

