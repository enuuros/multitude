/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef RADIANT_STRING_UTILS_HPP
#define RADIANT_STRING_UTILS_HPP

#include <list>
#include <sstream>
#include <string>

#include <Radiant/Trace.hpp>

namespace Radiant
{

  /// StringUtils is a collection of string manipulation functions.
  namespace StringUtils
  {

    typedef std::list<std::string>    StringList;
    typedef std::list<std::wstring>   WStringList;

    /// Remove non-visible characters from std::string.
    MTEXPORT void eraseNonVisibles(std::string & s);

    /// Convert std::string to std::wstring.
    MTEXPORT std::wstring stringTowstring(const std::string & str);

    /// Convert std::wstring to std::string
    /// @warning: non-ASCII characters may be lost in conversion.
    MTEXPORT std::string wstringTostring(const std::wstring & wstr);

    /// Split std::string into sub-strings.
    MTEXPORT void split(const std::string & s, const std::string & delim,
      StringList & out, bool skipEmpty = true);
    /// Split std::wstring into sub-strings.
    MTEXPORT void split(const std::wstring & ws, const std::wstring & delim,
      WStringList & out);

    /// Count the number of lines in the string.
    MTEXPORT int lineCount(const char * s);

    /// Convert utf8 string to wide string.
    MTEXPORT void utf8ToWString(std::wstring & dest, const std::string & src);
    /// Convert wide string to utf8 string.
    MTEXPORT void wstringToUtf8(std::string & dest, const std::wstring & src);
    /// Count the number of decoded unicode characters in a utf8 string.
    MTEXPORT int utf8DecodedLength(const std::string & src);
    /// Count the number of encoded utf8 bytes characters in a wide string.
    MTEXPORT int utf8EncodedLength(const std::wstring & src);

    template<class T>
      std::string stringify(T x) {
        std::ostringstream os;
        os << x;
        return os.str();
      }

    MTEXPORT const char * yesNo(bool yes);
  }
}

#endif
