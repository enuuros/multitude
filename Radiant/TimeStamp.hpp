/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef RADIANT_TIMESTAMP_HPP
#define RADIANT_TIMESTAMP_HPP

#include <Radiant/Export.hpp>

#ifdef WIN32
#include <WinPort.h>
#endif

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string>

namespace Radiant {
 
  /** A high-resolution time-stamp object. A time-stamp has 40 bits
     for the seconds and 24 bits for fractions of a second. The
     seconds are as they come from the UNIX clock: 1.1.1970 the
     counter was zero. */

  /// @todo remove suffix from create* functions
  class RADIANT_API TimeStamp
  {
  public:
    typedef int64_t type;
    
    enum {
      FRACTIONS_PER_SECOND = 0x1000000
    };

    static type ticksPerSecond() { return FRACTIONS_PER_SECOND; }
    static type ticksPerMinute() { return 1006632960; }
    static type ticksPerHour()   { return 60397977600ll; }
    static type ticksPerDay()    { return 1449551462400ll; }


    TimeStamp(type val = 0) : m_val(val) {}
    ~TimeStamp() {}

    type & value() { return m_val; }
    const type & value() const { return m_val; }

    void setSecondsD(double s)
    { m_val = (type) (s * FRACTIONS_PER_SECOND); }

    void setSecondsI(type s)
    { m_val = (s * FRACTIONS_PER_SECOND); }

    static TimeStamp createSecondsD(double s)
    { return (type) (s * FRACTIONS_PER_SECOND); }

    static TimeStamp createSecondsI(type s)
    { return (s << 24); }

    static TimeStamp createHoursD(double hours)
    { return (type) (hours * ticksPerHour()); }

    static TimeStamp createDaysD(double days)
    { return (type) (days * ticksPerDay()); }

    static TimeStamp createDaysI(type days)
    { return (type) (days * ticksPerDay()); }

    static TimeStamp createDHMS(int days, int hours, int minutes, int seconds)
    { 
      type tmp = ((type) seconds) << 24;
      tmp += ticksPerMinute() * (type) minutes;
      tmp += ticksPerHour() * (type) hours;
      return tmp + ticksPerDay() * (type) days;
    }

    static TimeStamp createDate(const char * date,
				const char * delim = "-",
				bool yearfirst = true);

    static TimeStamp createTime(const char * time,
				const char * delim = ":");

    static TimeStamp createDateTime(const char * date,
				    const char * delim,
				    bool yearfirst,
				    const char * time,
				    const char * timedelim);


    int64_t days() const { return m_val / ticksPerDay(); }
    double  daysD() const { return m_val / (double) ticksPerDay(); }
    int64_t seconds() const { return m_val >> 24; }
    int64_t fractions() const { return m_val & 0xFFFFFF; }
    double secondsD()  const { return m_val / (double) FRACTIONS_PER_SECOND; }
    double subSecondsD() const 
    { return (m_val & 0xFFFFFF) / (double) FRACTIONS_PER_SECOND; }
    int64_t subSecondsI() const 
    { return m_val & 0xFFFFFF; }
    double subSecondsUS() const { return 1000000.0 * subSecondsD(); }

    double secsTo(const TimeStamp & that)
    { return (that.m_val - m_val) / (double) FRACTIONS_PER_SECOND; }
    double usecsTo(const TimeStamp & that)
    { return (that.m_val - m_val) * 1000000.0/(double) FRACTIONS_PER_SECOND; }

    /** Returns the amount of time passed since this timestamp. */ 
    TimeStamp since() const { return getTime() - *this; }

    inline operator type & () { return m_val; }
    inline operator const type & () const { return m_val; }

    static type getTime();

    std::string asString() const;

  private:
    type m_val;
  };

}

#endif

