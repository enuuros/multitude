#ifndef RADIANT_STRING_UTILS_HPP
#define RADIANT_STRING_UTILS_HPP

#include <list>
#include <sstream>
#include <string>

#include <Radiant/Trace.hpp>

namespace Radiant
{

  /// StringUtils is a collection of string manipulation functions
  namespace StringUtils
  {

    typedef std::list<std::string>    StringList;
    typedef std::list<std::wstring>   WStringList;

    // Removes non-visible characters from string
    void eraseNonVisibles(std::string & s);

    // Converts std::string to std::wstring
    std::wstring stringTowstring(const std::string & str);

    // Converts std::wstring to std::string
    // Warning: non-ASCII characters may be lost in conversion
    std::string wstringTostring(const std::wstring & wstr);

    // Splits string into sub-strings.
    void split(const std::string & s, const std::string & delim,
      StringList & out, bool skipEmpty = true);
    void split(const std::wstring & ws, const std::wstring & delim,
	       WStringList & out);
    
    void utf8ToWString(std::wstring & dest, const std::string & src);
    void wstringToUtf8(std::string & dest, const std::wstring & src);
    /// Counts the number of decoded unicode characters in a utf8 string
    int utf8DecodedLength(const std::string & src);
    /// Counts the number of encoded utf8 bytes characters in a wide string
    int utf8EncodedLength(const std::wstring & src);
  }


  template<class T>
  std::string stringify(T x) {
    std::ostringstream os;
    os << x;
    return os.str();
  }

  const char * yesNo(bool yes);
}

#endif
