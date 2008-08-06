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

#include "Utils.hpp"

namespace Dyslexic
{

  namespace Utils
  {

    using namespace Radiant;
    using namespace StringUtils;

    void breakToLines(const std::wstring & ws, const float width,
      CPUBitmapFont & bitmapFont, WStringList & lines)
    {
      lines.clear();

      if(ws.empty())
      {
        return;
      }

      // First break the wstring at newlines

      std::wstring  delim;
      delim = W_NEWLINE;
      WStringList   wSub;

      split(ws, delim, wSub);

      // Break the resulting sub-wstrings to fit width

      delim = std::wstring(L" ");

      for(WStringList::iterator itSub = wSub.begin(); itSub != wSub.end(); itSub++)
      {
        // Split the sub-string into words

        WStringList   words;
        split(* itSub, delim, words);

        // Make the lines

        while(words.size())
        {
          // First try to fit it in as separate words

          const int   numWords = words.size();
          bool  got = false;
          BBox  bBox;
          for(int i = numWords; i >= 1 && !got; i--)
          {
            std::wstring  ln;
            WStringList::iterator   itWord = words.begin();
            for(int j = 0; j < i; j++, itWord++)
            {
              ln += * itWord;
            }
            bitmapFont.bbox((wchar_t *)(ln.data()), bBox);
            if(bBox.width() <= width)
            {
              lines.push_back(ln);
              for(int j = 0; j < i; j++)
              {
                words.pop_front();
              }
              got = true;
            }
          }

          if(got)
          {
            continue;
          }

          // Truncate the overlong word to fit width

          const std::wstring  word = words.front();
          const int   numChars = word.length();
          for(int i = numChars - 1; i >= 1 && !got; i--)
          {
            const std::wstring  ln = word.substr(0, i + 1);
            bitmapFont.bbox((wchar_t *)(ln.data()), bBox);
            if(bBox.width() <= width)
            {
              lines.push_back(ln);
              words.pop_front();
              words.push_front(word.substr(i + 1));
              got = true;
            }
          }
        }
      }

      // If last character is newline append empty line

      if(ws[ws.length() - 1] == W_NEWLINE)
      {
        lines.push_back(std::wstring(L""));
      }
    }

    void split(const std::wstring & ws, const std::wstring & delim,
      WStringList & out, const bool afterDelim)
    {
      out.clear();

      if(ws.empty())
      {
        return;
      }

      // Find first a delimiter

      std::wstring  wsCopy(ws);
      size_t  pos = wsCopy.find_first_of(delim);

      // Loop until no delimiters left

      if(afterDelim)
      // split string after delimiter
      {
        while(pos != wsCopy.npos)
        {
          out.push_back(wsCopy.substr(0, pos + 1));
          wsCopy.erase(0, pos + 1);
          pos = wsCopy.find_first_of(delim);
        }
      }
      else
      // split string before delimiter
      {
        while(pos != wsCopy.npos)
        {
          out.push_back(wsCopy.substr(0, pos));
          wsCopy.erase(0, pos);
          pos = wsCopy.find_first_of(delim, 1);
        }
      }

      // Push remainder of wstring onto list

      if(!wsCopy.empty())
      {
        out.push_back(wsCopy);
      }
    }

  }

}
