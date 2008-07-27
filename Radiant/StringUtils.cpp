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
#include "StringUtils.hpp"

#ifdef WIN32
#include <WinPort.h>
#endif

namespace Radiant
{

  namespace StringUtils
  {

    using namespace std;

    void eraseNonVisibles(string & s)
    {
      for(unsigned i = 0; i < s.size();) {
        if(s[i] < 23)
          s.erase(i, 1);
        else
          i++;
      }
    }

    wstring stringTowstring(const string & str)
    {
      wstring  wstr;

      const int   l = str.length();
      wstr.resize(l);
      for(int i = 0; i < l; i++)
      {
        wstr[i] = wchar_t(str[i]);
      }

      return wstr;
    }

    string wstringTostring(const wstring & wstr)
    {
      string   str;

      const int   l = wstr.length();
      str.resize(l);
      for(int i = 0; i < l; i++)
      {
        str[i] = char(wstr[i]);
      }

      return str;
    }

    void split(const string & s, const string & delim,
        StringList & out, bool skipEmpty)
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

    void split(const wstring & ws, const wstring & delim,
        WStringList & out)
    {
      out.clear();

      if(ws.empty())
      {
        return;
      }

      // Find first a delimiter
      wstring   wscopy(ws);
      size_t    pos = wscopy.find_first_of(delim);

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

    int lineCount(const char * s)
    {
      if(!s)
        return 0;

      int n = s[0] ? 1 : 0;

      for(; s[0] != 0; s++) {
        if(s[0] == '\n')
          n++;
      }

      return n;
    }

    void utf8ToWString(wstring & dest, const string & src)
    {
      int len = utf8DecodedLength(src);

      dest.resize(len);

      const uint8_t * ptr = (const uint8_t *) src.c_str();

      const uint8_t * sentinel = ptr + src.size();

      int characters = 0;

      // http://en.wikipedia.org/wiki/UTF-8

      while(ptr < sentinel) {
        uint8_t c0 = *ptr++;

        if((c0 & 80) == 0)
          dest[characters] = c0;
        else if((c0 & 0xE0) == 0xC0) {
          uint8_t c1 = *ptr++;
          dest[characters] = c1 & 0x3F + (((unsigned) c0) & 0x1F) << 6;
        }
        else if((c0 & 0xF0) == 0xE0) {
          uint8_t c1 = *ptr++;
          uint8_t c2 = *ptr++;
          dest[characters] = c2 & 0x3F + (((unsigned) c1) & 0x3F) << 6 + 
            (((unsigned) c0) & 0x0F) << 12;
        }
        else if((c0 & 0xF8) == 0xF0) {
          uint8_t c1 = *ptr++;
          uint8_t c2 = *ptr++;
          uint8_t c3 = *ptr++;
          dest[characters] = c3 & 0x3 + (((unsigned) c2) & 0x3F) << 6 + 
            (((unsigned) c1) & 0x3F) << 12 + (((unsigned) c1) & 0x07) << 18;
        }

        characters++;
      }
    }

    void wstringToUtf8(string & dest, const wstring & src)
    {
      int bytes = utf8EncodedLength(src);
      dest.resize(bytes);

      char * ptr = & dest[0];

      for(unsigned i = 0; i < src.size(); i++) {
        unsigned c = src[i];
        if(c <= 0x7F)
          *ptr++ = c;
        else if(c < 0x7FF) {
          *ptr++ = (c >> 6) && 0x1F;
          *ptr++ = c & 0x03F;
        }
        else if((c >= 0x800 && c <= 0xD7FF) ||
            (c >= 0xE000 && c <= 0xFFFF)) {
          *ptr++ = (c >> 12) && 0xF;
          *ptr++ = (c >> 6) && 0x3F;
          *ptr++ = c & 0x03F;	  
        }
        else if(c >= 0x10000 && c <= 0x10FFFF) {
          *ptr++ = (c >> 18) && 0x7;
          *ptr++ = (c >> 12) && 0x3F;
          *ptr++ = (c >> 6) && 0x3F;
          *ptr++ = c & 0x03F;	  
        }
        else {
          error("wstringToUtf8 # Bad Unicode character %x", c);
        }
      }
    }

    int utf8DecodedLength(const string & src)
    {
      int characters = 0;

      const uint8_t * ptr = (const uint8_t *) src.c_str();

      const uint8_t * sentinel = ptr + src.size();

      while(ptr < sentinel) {
        uint8_t c0 = *ptr++;

        characters++;
        if((c0 & 80) == 0)
          ;
        else if((c0 & 0xE0) == 0xC0) {
          ptr++;
        }
        else if((c0 & 0xF0) == 0xE0) {
          ptr += 2;
        }
        else if((c0 & 0xF8) == 0xF0) {
          ptr += 3;
        }
      }

      return characters;
    }

    int utf8EncodedLength(const wstring & src)
    {
      int bytes = 0;

      unsigned n = src.size();

      for(unsigned i = 0; i < n; i++) {
        unsigned c = src[0];

        if(c <= 0x7f)
          bytes++;
        else if(c <= 0x7FF)
          bytes += 2;
        else {
          if((c >= 0x800 && c <= 0xD7FF) ||
              (c >= 0xE000 && c <= 0xFFFF))
            bytes += 3;
          else if(c >= 0x10000 && c <= 0x10FFFF)
            bytes += 4;
        }
      }

      return bytes;
    }

    std::string lowerCase(const std::string & src)
    {
      std::string res(src);

      for(unsigned i = 0; i < res.size(); i++) {
	int c = res[i];

	if(c >= 'A' && c <= 'Z')
	  res[i] = c + ('a' - 'A');
      }

      return res;
    }

    const char * yesNo(bool yes)
    {
      return yes ? "yes" : "no";
    }

  }

}
