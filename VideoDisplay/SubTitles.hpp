/* COPYRIGHT
 *
 * This file is part of Valueable.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Valueable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */


#ifndef VIDEODISPLAY_SUBTITLES_HPP
#define VIDEODISPLAY_SUBTITLES_HPP

#include <Radiant/TimeStamp.hpp>

#include <string>
#include <vector>

namespace VideoDisplay {
  
  class SubTitles 
  {
  public:

    class Text
    {
    public:
      
      int lineCount() const
      {
	int n = 0;
	if(m_lines[0].size()) {
	  n = 1;
	  if(m_lines[1].size())
	    n = 2;
	}

	return n;
      }

      std::string m_lines[2];
      Radiant::TimeStamp m_begin;
      Radiant::TimeStamp m_end;
    };
    
    SubTitles();
    ~SubTitles();

    void update(Radiant::TimeStamp time);
    const Text * current();

    bool readSrt(const char * filename); 

    unsigned size() const { return m_texts.size(); }
    
  protected:
    
    std::vector<Text> m_texts;
    Text * m_current;
    int m_index;
  };
  
}

#endif

