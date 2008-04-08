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

#include "Utils.hpp"

namespace Dyslexic
{

  namespace Utils
  {

    using namespace Radiant;
    using namespace StringUtils;

    void breakToLines(const std::wstring & wStr, const float width,
      CPUBitmapFont & bitmapFont, WStringList & lines)
    {
      assert(width > 0.0f);

      lines.clear();

      if(wStr.empty())
      {
        return;
      }

      // First break the wstring at newlines

      std::wstring       delim;
      delim = wchar_t('\n');
      WStringList   wSub;

      split(wStr, delim, wSub);

      // Break the resulting sub-wstrings to fit width

      delim = wchar_t(' ');

      for(WStringList::iterator itSub = wSub.begin(); itSub != wSub.end(); itSub++)
      {
        // Split the sub-string into words

        WStringList   words;
        split(* itSub, delim, words);

        // Make the lines

        while(words.size())
        {
          // First try to fit it in as separate words

          const int numWords = words.size();
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
    }
  }

}
