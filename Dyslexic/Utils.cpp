#include "Utils.hpp"

namespace Dyslexic
{

  namespace Utils
  {

    using namespace std;
    using namespace Radiant;
    using namespace StringUtils;

    void breakToLines(const std::wstring & ws, const float width,
      CPUBitmapFont & bitmapFont, WStringList & lines)
    {
      assert(width > 0.0f);

      lines.clear();

      if(ws.empty())
      {
        return;
      }

      // Split the wstring into words

      const wchar_t   space = wchar_t(' ');
      const wchar_t   newLine = wchar_t('\n');
      wstring   delim;
      delim = space;
      StringUtils::WStringList   sub;
      split(ws, delim, sub);

      // Make the lines
      int     i = 0, j = 0, n = 0;
      bool    got = false;
      WStringList::iterator   it;
      std::wstring    tmp1, tmp2;
      BBox    bBox;
      while(sub.size())
      {
        // First try to fit it in as separate words

        n = sub.size();
        got = false;
        for(i = n; i >= 1 && !got; i--)
        {
          tmp1.clear();
          for(j = 0, it = sub.begin(); j < i; j++, it++)
          {
            tmp1 += *it;
          }
          bitmapFont.bbox((wchar_t *)(tmp1.data()), bBox);
          if(bBox.width() <= width)
          {
            lines.push_back(tmp1);
            for(j = 0; j < i; j++)
            {
              sub.pop_front();
            }
            got = true;
          }
        }

        if(got)
        {
          continue;
        }

        // Truncate the (overlong) word to fit in

        tmp2 = sub.front();
        n = tmp2.length();
        for(i = n - 1; i >= 1 && !got; i--)
        {
          tmp1 = tmp2.substr(0, i + 1);
          bitmapFont.bbox((wchar_t *)(tmp1.data()), bBox);
          if(bBox.width() <= width)
          {
            lines.push_back(tmp1);
            sub.pop_front();
            sub.push_front(tmp2.substr(i + 1));
            got = true;
          }
        }
      }

      // Process newline characters

      WStringList::iterator   its;
      for(its = lines.begin(); its != lines.end(); its++)
      {
        if(!its->empty())
        {
          int   pos = its->find(newLine);
          while(pos != int(string::npos))
          {
            std::wstring  left = its->substr(0, pos + 1);
            std::wstring  right = its->substr(pos + 1, its->length() - (pos + 1));
            lines.insert(its, left);
            * its = right;
            pos = its->find(newLine);
          }
        }
      }
    }
  }

}
