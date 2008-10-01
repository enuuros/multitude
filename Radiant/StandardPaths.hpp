/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef RADIANT_STANDARD_PATHS_HPP
#define RADIANT_STANDARD_PATHS_HPP

#include <string>

namespace Radiant
{

  /// Default font search paths (no trailing /) 
#ifdef WIN32
  const std::string STD_FONT_PATHS("../../../share/MultiTouch/Fonts;C:/WINDOWS/Fonts;.");
#else
  const std::string STD_FONT_PATHS("../../share/MultiTouch/Fonts;/usr/share/fonts/truetype/ttf-dejavu;.");
#endif

}

#endif
