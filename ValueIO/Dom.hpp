/* COPYRIGHT
 *
 * This file is part of ValueIO.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ValueIO.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUEIO_DOM_HPP
#define VALUEIO_DOM_HPP

#include <xercesc/dom/DOM.hpp>

#include <string>

namespace ValueIO {

  XERCES_CPP_NAMESPACE_USE;

  /// An string object that wraps XMLCh strings
  /** This object is typically used when converting from normal
      character strings to Xerces strings.  */
  class XWStr
  {
  public :
    XWStr(const char * cstr)
      : m_str(cstr ? XMLString::transcode(cstr) : 0) {}

    XWStr(const XWStr & that)
      : m_str(XMLString::replicate(that.m_str)) {}

    XWStr(XMLCh * str)
      : m_str(str) {}
    
    ~XWStr() { XMLString::release( & m_str); }
    
    /// Return a pointer to the actual string.
    const XMLCh* str() const { return m_str; }
    static int stringSize(const XMLCh* xstr) 
    { 
      if(!xstr) return 0;
      int tmp = XMLString::stringLen(xstr); 
      if(!tmp) return 0;
      return tmp + 1; // One for the trailing null...
    }

    int size() const 
    { return stringSize(m_str); }

    XWStr & operator = (const XWStr & that) 
    { 
      XMLString::release( & m_str);
      m_str = XMLString::replicate(that.m_str); 
      return * this;
    }

  private :
    XMLCh * m_str;
  };

  /// Creates and returns a temporary Xerces unicode string.
#define XW(x) ValueIO::XWStr(x).str()
#define VALUEIO_XW(x) ValueIO::XWStr(x).str()
#define SXW(x) (const wchar_t *) ValueIO::XWStr(x).str()
#define VALUEIO_SXW(x) (const wchar_t *) ValueIO::XWStr(x).str()

  /// An string object that wraps normal character strings
  /** This object is typically used when converting from Xerces
      strings to normal character strings.  */
  class XCStr 
  {
  public:
    XCStr(const XMLCh * xmlstr) 
      : m_str(xmlstr ? XMLString::transcode(xmlstr) : 0)
    {}

    XCStr(const XCStr & that) 
    {
      m_str = that.m_str ? XMLString::replicate(that.m_str) : 0;
    }

    ~XCStr() { if(m_str) XMLString::release( & m_str); }

    const char * str() const { return m_str; }
    char * str() { return m_str; }

    XCStr & operator = (const XCStr & that) 
    { 
      XMLString::release( & m_str);
      m_str = XMLString::replicate(that.m_str); 
      return * this;
    }

    XCStr & operator = (const  XMLCh * xmlstr)
    { 
      XMLString::release( & m_str);
      m_str = xmlstr ? XMLString::transcode(xmlstr) : 0;
      return * this;
    }

  protected:
    char * m_str;
    
  };

  /// Creates and returns a temporary character string.
#define XC(x) ValueIO::XCStr(x).str()

  inline void towstd(std::wstring & target, const XMLCh * source)
  {
    assert(sizeof(wchar_t) == sizeof(XMLCh));
    if(!source)
      target.clear();
    else
      target.assign((const wchar_t *)source, XWStr::stringSize(source));
  }

  inline void towstd(std::wstring & target, const char * source)
  {
    const XWStr tmp(source);
    towstd(target, tmp.str());
  }
  
  template <class T>
  inline std::wstring toString(const T & x) { return 0; }

  template <>
  inline std::wstring toString<bool>(const bool & x) 
  { 
    char buf[12];
    sprintf(buf, "%d", (int) x); 
    return std::wstring(SXW(buf), strlen(buf));
  }

  template <>
  inline std::wstring toString<int64_t>(const int64_t & x) 
  { 
    char buf[12];
    sprintf(buf, "%lld", (long long) x); 
    return std::wstring(SXW(buf), strlen(buf));
  }

  template <>
  inline std::wstring toString<int>(const int & x) 
  { 
    char buf[12];
    sprintf(buf, "%d", x); 
    return std::wstring(SXW(buf), strlen(buf));
  }

  template <>
  inline std::wstring toString<float>(const float & x) 
  { 
    char buf[12];
    sprintf(buf, "%f", x); 
    return std::wstring(SXW(buf), strlen(buf));
  }

  template <>
  inline std::wstring toString<double>(const double & x) 
  { 
    char buf[12];
    sprintf(buf, "%lf", x); 
    return std::wstring(SXW(buf), strlen(buf));
  }

  template <>
  inline std::wstring toString<std::wstring>(const std::wstring & str) 
  { return str; }

  int toInt(const XMLCh *, bool *);
  long long toLongLong(const XMLCh *, bool *);
  float toFloat(const XMLCh *, bool *);
  double toDouble(const XMLCh *, bool *);

  /* template <class T>
     inline T toValue(const XMLCh * str) 
     { return str.toDouble(); }
  */

  inline int toValue(const XMLCh * str)
  { return atoi(XC(str)); }

  void addTextChild(DOMElement *, DOMDocument *doc,
		    const XMLCh *tag,  const XMLCh *value);
  void addTextChild(DOMElement *, DOMDocument *doc,
		    const XMLCh *tag,  const std::wstring & value);
  void addTextChild(DOMElement *, DOMDocument *doc,
		       const XMLCh *tag,  int32_t value);
  void addTextChild(DOMElement *, DOMDocument *doc,
		       const XMLCh *tag,  int64_t value);
  void addTextChild(DOMElement *, DOMDocument *doc,
		       const XMLCh *tag,  double value);
  void addTextChild(DOMElement *, DOMDocument *doc,
		    const XMLCh *tag, const float * values, int n);
  void addTextChild(DOMElement *, DOMDocument *doc,
		    const XMLCh *tag, const int * values, int n);
  void addTextChild(DOMElement *, DOMDocument *doc,
        const XMLCh *tag, const unsigned char * values, int n);
  
  DOMNode * namedItem(DOMElement * e, const XMLCh * tag);

  std::wstring     getString(DOMElement *, const XMLCh *tag, 
			     const XMLCh * def = 0);
  double           getDouble(DOMElement *, const XMLCh *tag, 
			     double def=0.0f);
  int              getInt(DOMElement *, const XMLCh *tag, int def=0);
  long             getLong(DOMElement *, const XMLCh *tag, long def=0);

  /// Get a named (tag) value from an element, that is converted to type T.
  template <class T>
  inline T get(DOMElement *, const XMLCh *tag, T def=0);

  /* template <>
  inline std::wstring get<std::wstring>(DOMElement *e, const XMLCh *tag, std::wstring def)
  {
    return getString(e, tag, def);
  }
  */

  template <>
  inline bool get<bool>(DOMElement *e, const XMLCh *tag, bool def)
  {
    return getInt(e, tag, def);
  }

  template <>
  inline int get<int>(DOMElement *e, const XMLCh *tag, int def)
  {
    return getInt(e, tag, def);
  }

  template <>
  inline uint get<uint>(DOMElement *e, const XMLCh *tag, uint def)
  {
    return getInt(e, tag, def);
  }

  template <>
  inline long get<long>(DOMElement *e, const XMLCh *tag, long def)
  {
    return getLong(e, tag, def);
  }

  template <>
  inline double get<double>(DOMElement *e, const XMLCh *tag, double def)
  {
    return getDouble(e, tag, def);
  }

  template <>
  inline float get<float>(DOMElement *e, const XMLCh *tag, float def)
  {
    return getDouble(e, tag, def);
  }

}

#endif
