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

#include "IO.hpp"

#include "Dom.hpp"

#include "HasValues.hpp"

namespace ValueIO {

    float IO::getFloat(void * , int ) 
    { 
      Radiant::error("IO::getFloat # conversion not available"); 
      return 0.f; 
    }

    void IO::setFloat(void *, int , float)
    {
      Radiant::error("IO::setFloat # conversion not available");      
    }

    Nimble::Vector4 IO::getVector4(void *) 
    {
      Radiant::error("IO::getVector4 # conversion not available");
      return Nimble::Vector4(0.f, 0.f, 0.f, 0.f);
    }

    void IO::setVector4(void *, const Nimble::Vector4 &)
    {
      Radiant::error("IO::getVector4 # conversion not available");
    }

    void IO::setVector2(void * , const Nimble::Vector2 & )
    {
      Radiant::error("IO::setVector2 # conversion not available");
    }

    Nimble::Vector2 IO::getVector2(void *)
    {
      Radiant::error("IO::getVector2 # conversion not available");
      return Nimble::Vector2(0.f, 0.f);
    }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOStdString::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    std::string & val = * (std::string *) ptr;
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    XCStr tmp(text->getNodeValue());

    val = tmp.str();

    return true;
  }

  DOMNode * IOStdString::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    const std::string & val = * (std::string *) ptr;
    XWStr tmp(XMLString::transcode(val.c_str()));
    return doc->createTextNode(tmp.str());
  }
  
  IOStdString IOStdString::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOStdWString::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    std::wstring &  val = * (std::wstring *) (ptr);
    DOMText * const text = dynamic_cast<DOMText *> (e);

    if(!text)
    {
      return false;
    }

    const XMLCh * const   xmlStr = text->getData();
    const int       numChars = text->getLength();
    val.resize(numChars);
    int i = 0;
    for(i = 0; i < numChars; i++)
    {
      val[i] = wchar_t(xmlStr[i]);
    }

    return true;
  }
  
  DOMNode * IOStdWString::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    const std::wstring &  val = * (std::wstring *) (ptr);
    
    const int       numChars = val.size();
    XMLCh * const   xmlStr = new XMLCh[numChars + 1];
    assert(xmlStr);
    int i = 0;
    for(i = 0; i < numChars; i++)
    {
      xmlStr[i] = XMLCh(val[i]);
    }
    xmlStr[i] = '\0';

    const XWStr     xwStr(XMLString::transcode(xmlStr));
 
    return doc->createTextNode(xwStr.str());
  }
  
  IOStdWString IOStdWString::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOHasValues::readDom(void * ptr, HasValues *, DOMNode *, DOMElement *e)
  {
    HasValues * obj = (HasValues *) ptr;
    return obj->readDom(e);
  }
  
  DOMNode * IOHasValues::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    HasValues * obj = (HasValues *) ptr;
    return obj->writeDom(doc);
  }
    
  IOHasValues IOHasValues::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOInt32::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);
    if(!text) return false;
    XCStr tmp(text->getNodeValue());
    * ((int32_t *) ptr) = atoi(tmp.str());
    return true;
  }
  
  DOMNode * IOInt32::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    char buf[32];
    sprintf(buf, "%d", * (int32_t *) ptr);
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }
    
  IOInt32 IOInt32::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOInt64::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);
    if(!text) return false;
    XCStr tmp(text->getNodeValue());
    * ((int64_t *) ptr) = atoi(tmp.str());
    return true;
  }
  
  DOMNode * IOInt64::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    char buf[32];
    sprintf(buf, "%lld", (long long) * (int64_t *) ptr);
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }
    
  IOInt64 IOInt64::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOFloat32::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    XCStr tmp(text->getNodeValue());

    * ((float *) ptr) = atof(tmp.str());
    
    return true;
  }
  
  DOMNode * IOFloat32::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    char buf[32];
    sprintf(buf, "%f", * (float *) ptr ); 
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }
    
  IOFloat32 IOFloat32::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOFloat64::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    XCStr tmp(text->getNodeValue());

    * ((double *) ptr) = atof(tmp.str());
    
    return true;
  }
  
  DOMNode * IOFloat64::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    char buf[32];
    sprintf(buf, "%lf", * (double *) ptr ); 
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }
    
  IOFloat64 IOFloat64::instance;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOVectorFloat32::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    XCStr tmp(text->getNodeValue());

    char * p1 = tmp.str();
    char * p2 = p1;
    float * val = (float *) ptr;

    for(int i = 0; i < m_size; i++) {
      val[i] = strtof(p1, &p2);
      p1 = p2;
    }
    
    return true;
  }
  
  DOMNode * IOVectorFloat32::writeDom(void * ptr, HasValues *,DOMDocument * doc)
  {
    float * val = (float *) ptr;
    char buf[1024];
    char *p = buf;
    for(int i = 0; i < m_size; i++) {
      sprintf(p, "%f ", val[i]);
      p += strlen(p);
    }
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }

  IOVectorFloat32 IOVectorFloat32::instance2(2);
  IOVectorFloat32 IOVectorFloat32::instance3(3);
  IOVectorFloat32 IOVectorFloat32::instance4(4);

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOVectorInt32::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    XCStr tmp(text->getNodeValue());

    char * p1 = tmp.str();
    char * p2 = p1;
    int * val = (int *) ptr;

    for(int i = 0; i < m_size; i++) {
      val[i] = strtol(p1, &p2, 10);
      p1 = p2;
    }
    
    return true;
  }
  
  DOMNode * IOVectorInt32::writeDom(void * ptr, HasValues *,DOMDocument * doc)
  {
    int32_t * val = (int32_t *) ptr;
    char buf[1024];
    char *p = buf;
    for(int i = 0; i < m_size; i++) {
      sprintf(p, "%d ", (int) val[i]);
      p += strlen(p);
    }
    XWStr tmp(buf);
    return doc->createTextNode(tmp.str());    
  }

  IOVectorInt32 IOVectorInt32::instance2(2);
  IOVectorInt32 IOVectorInt32::instance3(3);
  IOVectorInt32 IOVectorInt32::instance4(4);

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool IOStdVectorVectorFloat32::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    DOMNode * n = e->getFirstChild();

    float vect[10];

    IOVectorFloat32 helper(m_size);

    bool ok = true;

    while(n) {

      XCStr tmp(n->getNodeName());
      
      if(strcmp(tmp.str(), "v") != 0)
	ok = false;
      else {
	bool ok2 = helper.readDom(vect, 0, n->getFirstChild(), 0);
	if(ok2) {
	  if(m_size == 2)
	    ((std::vector<Nimble::Vector2> *) ptr)->
	      push_back(Nimble::Vector2(vect));
	  if(m_size == 3)
	    ((std::vector<Nimble::Vector3> *) ptr)->
	      push_back(Nimble::Vector3(vect));
	  if(m_size == 4)
	    ((std::vector<Nimble::Vector4> *) ptr)->
	      push_back(Nimble::Vector4(vect));
	}
	ok = ok && ok2;
      }

      n = n->getNextSibling();
    }

    return ok;
  }

  DOMNode * IOStdVectorVectorFloat32::writeDom
  (void * ptr, HasValues *, DOMDocument * doc)
  {
    DOMElement * e = doc->createElement(XW("array"));

    float * src = 0;
    int n = 0;

    if(m_size == 2) {
      std::vector<Nimble::Vector2> * vect=(std::vector<Nimble::Vector2> *) ptr;
      n = vect->size();
      src = (*vect)[0].data();
    }
    else if(m_size == 3) {
      std::vector<Nimble::Vector3> * vect=(std::vector<Nimble::Vector3> *) ptr;
      n = vect->size();
      src = (*vect)[0].data();
    }
    else if(m_size == 4) {
      std::vector<Nimble::Vector4> * vect=(std::vector<Nimble::Vector4> *) ptr;
      n = vect->size();
      src = (*vect)[0].data();
    }

    XWStr vv("v");

    for(int i = 0; i < n; i++) {
      char buf[256];
      char * ptr = buf;
      for(int j = 0; j < m_size; j++) {
	sprintf(ptr, "%f ", *src++);
	ptr += strlen(ptr);
      }

      DOMElement * e2 = doc->createElement(vv.str());
      e2->appendChild(doc->createTextNode(XW(buf)));
      e->appendChild(e2);
    }

    return e;
  }

  IOVectorFloat32 IOStdVectorVectorFloat32::instance2(2);
  IOVectorFloat32 IOStdVectorVectorFloat32::instance3(3);
  IOVectorFloat32 IOStdVectorVectorFloat32::instance4(4);

}
