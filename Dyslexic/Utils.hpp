/* COPYRIGHT
 *
 * This file is part of Dyslexic.
 *
 * Copyright: MultiTouch Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2007-2008.
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from MultiTouch Oy.
 *
 * See file "Dyslexic.hpp" for authors and more details.
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
