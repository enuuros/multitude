/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Dyslexic.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DYSLEXIC_UTILS_HPP
#define DYSLEXIC_UTILS_HPP

#include <Dyslexic/CPUBitmapFont.hpp>

#include <Radiant/StringUtils.hpp>

namespace Dyslexic
{

  namespace Utils
  {

    /**
      * @brief Breaks wstring into lines.
      * The lines will be less than or equal to the specified width when displayed
      * in the given font.
      * @param wStr
      * @param width
      * @param bitmapFont
      * @param lines Reference to list to receive the lines.
      * @note Newline characters are retained in the output.
      */
    void breakToLines(const std::wstring & wStr, const float width,
      CPUBitmapFont & bitmapFont, Radiant::StringUtils::WStringList & lines);

  }

}

#endif
