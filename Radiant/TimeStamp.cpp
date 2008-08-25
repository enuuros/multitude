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


#include "TimeStamp.hpp"

#include "StringUtils.hpp"

namespace Radiant {

  TimeStamp TimeStamp::createDate(const char * date,
				  const char * delim,
				  bool yearfirst)
  {
    if(!date)
      return 0;

    StringUtils::StringList segments;
    StringUtils::split(date, delim, segments);

    if(segments.size() != 3) {
      return TimeStamp(0);
    }

    int vals[3];

    StringUtils::StringList::iterator it = segments.begin();

    for(int i = 0; i < 3; i++) {
      vals[i] = atoi((*it).c_str());
      it++;
    }

    int year, month, day;

    month = vals[1]; // Month is always in the middle

    if(yearfirst) {
      year = vals[0];
      day = vals[2];
    }
    else {
      year = vals[2];
      day = vals[0];
    }

    struct tm tms;

    bzero(& tms, sizeof(tms));

    tms.tm_year = year - 1900;
    tms.tm_mon = month - 1;
    tms.tm_mday = day;
    
    time_t tval = mktime(&tms);

    //trace("tval as ctime = %s (%d %d %d)", ctime( & tval), year, month, day);

    return TimeStamp(tval * ticksPerSecond());
  }

  TimeStamp TimeStamp::createTime(const char * time,
				  const char * delim)
  {
    if(!time)
      return 0;

    StringUtils::StringList segments;
    StringUtils::split(time, delim, segments);
    
    if(segments.size() != 3) {
      return TimeStamp(0);
    }

    int vals[3];

    StringUtils::StringList::iterator it = segments.begin();

    for(int i = 0; i < 3; i++) {
      vals[i] = atoi((*it).c_str());
      it++;
    }

    int hour = vals[0];
    int minute = vals[1];
    int second = vals[2];
    
    return createDHMS(0, hour, minute, second);
  }

  TimeStamp TimeStamp::createDateTime(const char * date,
				      const char * delim,
				      bool yearfirst,
				      const char * time,
				      const char * timedelim)
  {
    return createDate(date, delim, yearfirst) + createTime(time, timedelim);
  }

}
