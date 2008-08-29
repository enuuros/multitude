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

#include <Radiant/DateTime.hpp>

#include <cassert>

namespace Radiant {

  DateTime::DateTime()
    : m_year(0),
      m_month(0),
      m_monthDay(0),
      m_weekDay(0),
      m_hour(0),
      m_minute(0),
      m_second(0),
      m_summerTime(false)
  {}

  DateTime::DateTime(const TimeStamp & t)
  {
    time_t secs = t.seconds();
    struct tm tms;
    localtime_r(& secs, & tms);

    m_year     = tms.tm_year + 1900;
    m_month    = tms.tm_mon;
    m_monthDay = tms.tm_mday - 1;
    m_weekDay  = tms.tm_wday;
    m_hour     = tms.tm_hour;
    m_minute   = tms.tm_min;
    m_second   = tms.tm_sec;
    m_summerTime = tms.tm_isdst;
  }

  DateTime::~DateTime()
  {}

  void DateTime::clearTime()
  {
    m_hour   = 0;
    m_minute = 0;
    m_second = 0;
  }

  void DateTime::toNextMonth()
  {
    m_month++;
    if(m_month >= 12) {
      m_month -= 12;
      m_year++;
    }
  }

  void DateTime::toNextMonthDay()
  {
    m_monthDay++;
    if(m_monthDay >= daysInMonth()) {
      m_monthDay = 0;
      m_month++;
      if(m_month >= 12) {
	m_month = 0;
	m_year++;
      }
    }
  }

  int DateTime::daysInMonth(int month, int year)
  {
    static const int table[12] = {
      31, 28, 31, 30, 31, 30,
      31, 31, 30, 31, 30, 31
    };

    assert((unsigned) month < 12);

    int days = table[month];
    if(month == 1) {
      // February
      if((year & 0x3) == 0)
	days = 29;
    }

    return days;
  }

  int DateTime::daysInMonth()
  {
    return daysInMonth(month(), year());
  }

  TimeStamp DateTime::asTimeStamp() const
  {
    struct tm tms;

    bzero( & tms, sizeof(tms));

    tms.tm_year = m_year - 1900;
    tms.tm_mon  = m_month;
    tms.tm_mday = m_monthDay + 1;
    tms.tm_wday = m_weekDay;
    tms.tm_hour = m_hour;
    tms.tm_min  = m_minute;
    tms.tm_sec  = m_second;
    tms.tm_isdst= m_summerTime;
    time_t tval = mktime(&tms);

    //trace("tval as ctime = %s (%d %d %d)", ctime( & tval), year, month, day);

    return TimeStamp(tval * TimeStamp::ticksPerSecond());
  }
}