#ifndef DYSLEXIC_UTILS_HPP
#define DYSLEXIC_UTILS_HPP

#include <Dyslexic/CPUBitmapFont.hpp>

#include <Radiant/StringUtils.hpp>

namespace Dyslexic
{

  namespace Utils
  {

    // Breaks wstring into lines that will be less than or equal to the
    // specified width when displayed in the given font
    void breakToLines(const std::wstring & ws, const float width,
      CPUBitmapFont & bitmapFont, Radiant::StringUtils::WStringList & lines);

  }

}

#endif
