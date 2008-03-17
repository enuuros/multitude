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

#include "Dom.hpp"

namespace ValueIO {

  int toInt(const XMLCh * str, bool * ok)
  {
    XCStr tmp(str);
    char * endptr = (char *) tmp.str();
    int res = strtol(tmp.str(), & endptr, 10);
    if(tmp.str() == endptr && ok)
      *ok = false;
    return res;
  }

  long long toLongLong(const XMLCh * str, bool * ok)
  {
    XCStr tmp(str);
    char * endptr = (char *) tmp.str();
    long long res = strtoll(tmp.str(), & endptr, 10);
    if(tmp.str() == endptr && ok)
      *ok = false;
    return res;
  }

  float toFloat(const XMLCh * str, bool * ok)
  {
    return toDouble(str, ok);
  }

  double toDouble(const XMLCh * str, bool * ok)
  {
    XCStr tmp(str);
    char * endptr = (char *) tmp.str();
    double res = strtod(tmp.str(), & endptr);
    if(tmp.str() == endptr && ok)
      *ok = false;
    return res;
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		       const XMLCh * tag, const XMLCh * value)
  {
    DOMElement * si = doc->createElement(tag);
    si->appendChild(doc->createTextNode(value));
    base->appendChild(si);
  }

  void addTextChild(DOMElement * e, DOMDocument *doc,
		    const XMLCh *tag,  const std::wstring & value)
  {
    int n = value.size();
    if(n) {
      n--;
      if(value[n] != 0) {
	printf("BAD WSTRING (%c)", (char) value[n]);
	fflush(0);
	assert(value[n] == 0);
      }

      addTextChild(e, doc, tag, value.c_str());
    }
    else {
      XMLCh nothing = 0;
      addTextChild(e, doc, tag, & nothing);
    }
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		       const XMLCh * tag, int32_t value)
  {
    addTextChild(base, doc, tag, (const XMLCh *) toString(value).c_str());
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		       const XMLCh * tag,  int64_t value)
  {
    addTextChild(base, doc, tag, (const XMLCh *) toString(value).c_str());
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		       const XMLCh * tag,  double value)
  {
    addTextChild(base, doc, tag, (const XMLCh *) toString(value).c_str());
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		    const XMLCh * tag,  const float * values, int n)
  {
    std::wstring tmp;
    for(int i = 0; i < n; i++)
      tmp += wchar_t(' ') + toString(values[i]);
    addTextChild(base, doc, tag, (const XMLCh *) tmp.c_str());
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
		    const XMLCh * tag,  const int * values, int n)
  {
    std::wstring tmp;
    for(int i = 0; i < n; i++)
      tmp += wchar_t(' ') + toString(values[i]);
    addTextChild(base, doc, tag, (const XMLCh *) tmp.c_str());
  }

  void addTextChild(DOMElement *base, DOMDocument * doc,
        const XMLCh * tag, const unsigned char * values, int n)
  {
    std::wstring tmp;
    for(int i = 0; i < n; i++)
      tmp += wchar_t(' ') + toString((int) values[i]);
    addTextChild(base, doc, tag, (const XMLCh *) tmp.c_str());
  }

  /** Find the first child node with given tag. */
  DOMNode * namedItem(DOMElement * e, const XMLCh * tag)
  {
    DOMNode * n = e->getFirstChild();

    while(n) {
      if(XMLString::equals(tag, n->getNodeName()))
	return n;
      n = n->getNextSibling();
    }

    return 0;
  }
  
  std::wstring getString(DOMElement * e, const XMLCh * tag, 
			 const XMLCh * def)
  {
    DOMNode * n = namedItem(e, tag);
    if(!n) std::wstring((wchar_t *) def, XMLString::stringLen(def));

    DOMNode *t = n->getFirstChild();
    if(!t) std::wstring((wchar_t *) def, XMLString::stringLen(def));

    const XMLCh * buf = t->getNodeValue();
    return std::wstring((wchar_t *) buf, XMLString::stringLen(buf));
  }

  double  getDouble(DOMElement *e, const XMLCh * tag, double def)
  {
    DOMNode * c = namedItem(e, tag);

    if(!c) return def;

    DOMNode * t = c->getFirstChild();
    
    if(t) return def;

    XCStr tmp(t->getNodeValue());

    const char * str = tmp.str();

    char * endptr = (char *) str;

    double val = strtod(str, & endptr);

    return str != endptr ? val : def;
  }

  int getInt(DOMElement *e, const XMLCh * tag, int def)
  {
    return getLong(e, tag, def);
  }

  long getLong(DOMElement *e, const XMLCh * tag, long def)
  {
    DOMNode * c = namedItem(e, tag);

    if(!c) return def;

    DOMNode * t = c->getFirstChild();
    
    if(t) return def;

    XCStr tmp(t->getNodeValue());

    const char * str = tmp.str();

    char * endptr = (char *) str;
    
    long val = strtol(str, & endptr, 10);

    return str != endptr ? val : def;
  }


}
