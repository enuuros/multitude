#include "StringUtils.hpp"

namespace Radiant
{

  namespace StringUtils
  {

    using namespace std;

    void eraseNonVisibles(std::string & s)
    {
      for(unsigned i = 0; i < s.size();) {
        if(s[i] < 23)
          s.erase(i, 1);
        else
          i++;
      }
    }

    std::wstring stringTowstring(const std::string & str)
    {
      std::wstring  wstr;

      const int     l = str.length();
      wstr.resize(l);
      for(register int i = 0; i < l; i++)
      {
        wstr[i] = wchar_t(str[i]);
      }

      return wstr;
    }

    std::string wstringTostring(const std::wstring & wstr)
    {
      std::string   str;

      const int     l = wstr.length();
      str.resize(l);
      for(register int i = 0; i < l; i++)
      {
        str[i] = char(wstr[i]);
      }

      return str;
    }

    void split(const string & s, const string & delim, StringList & out, bool skipEmpty)
    {
      string::size_type offset = 0;
      string::size_type index = 0;

      index = s.find(delim, offset);

      while(index != string::npos) {
        string piece = s.substr(offset, index - offset);

        if(skipEmpty && piece != string(""))
          out.push_back(piece);

        offset += index - offset + delim.length();
        index = s.find(delim, offset);
      }

      string piece = s.substr(offset);

      if(skipEmpty && piece != string(""))
        out.push_back(piece);
    }

    void split(const std::wstring & ws, const std::wstring & delim, WStringList & out)
    {
      out.clear();

      if(ws.empty())
      {
        return;
      }

      // Find first a delimiter
      std::wstring  wscopy(ws);
      size_t        pos = wscopy.find_first_of(delim);

      // Loop until no delimiters left
      while(pos != wscopy.npos)
      {
        out.push_back(wscopy.substr(0, pos + 1));
        wscopy.erase(0, pos + 1);
        pos = wscopy.find_first_of(delim);
      }

      // Push remainder of wstring onto list
      if(!wscopy.empty())
      {
        out.push_back(wscopy);
      }
    }
  }

  const char * yesNo(bool yes)
  {
    return yes ? "yes" : "no";
  }

}
