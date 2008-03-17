#ifndef RADIANT_STRING_UTILS_HPP
#define RADIANT_STRING_UTILS_HPP

#include <list>
#include <sstream>
#include <string>

#include <Radiant/Trace.hpp>

namespace Radiant
{

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
