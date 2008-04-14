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

#include "HasValues.hpp"

#include "Dom.hpp"
#include "DomStore.hpp"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <Radiant/Trace.hpp>

#include <assert.h>

#include <typeinfo>

namespace ValueIO {

  using Radiant::error;
  using Radiant::trace;

  class TestOut : public HasValues
  {
  public:
    TestOut(bool zeros = false)
      : m_int1(zeros ? 0 : 123),
	m_int2(zeros ? 0 : 456),
	m_float(zeros ? 0.0f : 44.33f),
	m_double(zeros ? 0.0 : 22.11)
    {
      addValue("int1", m_int1);
      addValue("int2", m_int2);
      addValue("float", m_float);
      addValue("double", m_double);
      addValue("str", & m_str);
      addValue("vec", m_vec);

      setName("TESTOBJ");

      // towstd(m_str, "ABCDEFG");
      if(!zeros) {
	m_str = "ABCDEFG";
	m_vec.make(12, 34, 56);
      }
      else
	m_vec.clear();

      printf("wchar: %d, XMLCh: %d\n",
	     (int) sizeof(wchar_t), (int) sizeof(XMLCh));
    }
    virtual ~TestOut() {}

  private:
    int m_int1;
    int m_int2;
    float  m_float;
    double m_double;
    std::string m_str;
    Nimble::Vector3 m_vec;
  };

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////


  HasValues::HasValues()
  {}

  HasValues::~HasValues()
  {}

  void HasValues::setName(const std::string & name)
  {
    m_name = name;
  }

  void HasValues::setName(const char * name)
  {
    m_name = name;
    // return;
    /* XWStr tmp(name);
       towstd(m_name, tmp.str()); */
  }

  void HasValues::forgetValue(const std::string & id)
  {
    iterator it = m_values.find(id);
    if(it != m_values.end())
      m_values.erase(it);
  }

  HasValues::Ptr HasValues::value(const std::string & id)
  {
    iterator it = m_values.find(id);
    
    if(it != m_values.end())
      return (*it).second;

    return Ptr();
  }

  HasValues::Ptr HasValues::value(const XMLCh * id)
  {
    char * cstr = XMLString::transcode(id);
    std::string tmp(cstr);
    XMLString::release(&cstr);
    return value(tmp);
  }

  DOMElement * HasValues::writeDom(DOMDocument * doc)
  {
    init();

    DOMElement * ev = doc->createElement(XWStr("values").str());
  
    for(iterator it = m_values.begin(); it != m_values.end(); it++) {

      Ptr  ptr = (*it).second;
      XWStr xstr((*it).first.c_str());
      assert(ptr.m_io);

      DOMNode * n = ptr.m_io->writeDom(ptr.m_ptr, this, doc);

      if(n) {
        DOMElement * e = doc->createElement(xstr.str());
        e->appendChild(n);
        ev->appendChild(e);
        //trace("HasValues::writeDom # Wrote item \"%s\"", (*it).first.c_str());
      }
      else 
        error("HasValues::writeDom # When writing item \"%s\"",
            (*it).first.c_str());
    }

    return ev;
  }

  bool HasValues::readDom(DOMElement * e)
  {
    /* if(e.tagName() != m_name)
       return false; */

    init();

    bool ok = true;
  
    DOMNode * ev = namedItem(e, XW("values"));

    if(!ev) {
      error("HasValues::readDom # no children");
      return false;
    }

    // trace("HasValues::readDom");

    DOMNode * n = ev->getFirstChild();
    
    while(n) {

      DOMElement * tmp = dynamic_cast<DOMElement *> (n);

      DOMNode * nc = n->getFirstChild();

      if(tmp && nc) {
        const XMLCh * tag = tmp->getNodeName();

	// trace("HasValues::readDom # %s # %s", typeid(*this).name(), XC(tag));

	Ptr ptr = value(tag);

	if(!ptr.m_io)
	  ok = ok && readValue(tmp);
	else {
	  bool ok2 = ptr.m_io->readDom(ptr.m_ptr, this, nc, tmp);
    if(!ok2)
      error("HasValues::readDom # in reading %s", XC(tag));
    ok = ok && ok2;
   }
      }
	
      n = n->getNextSibling();
    }

    return ok;
  }

  bool HasValues::load(const char * filename)
  {
    try {
    init();

    DOMStore in;
    bool ok = false;

    if(in.prepareLoad(filename)) {
      ok = readDom(in.doc()->getDocumentElement());
    }
    else
      error("HasValues::load");

    in.finishLoad();

    trace("HasValues::load # %d", (int) ok);

    return ok;
    } catch(DOMException e) {
      error("HasValues::load # Xerces excetion: %s", XC(e.getMessage()));
      return false;
    }
  }

  
  bool HasValues::save(const char * filename)
  {
    const char * fname = "HasValues::save";

    try {

    init();

    DOMStore out;

    DOMElement * root = out.prepareSave(filename, m_name.c_str());

    if(!root) {
      error("HasValues::save # couldn't create document root");
      return false;
    }
    
    root->appendChild(writeDom(out.doc()));
    bool ok = out.finishSave();

    if (!ok)
      error("%s # Could not serialize to %s", fname, filename);

    return ok;
    } catch(DOMException e) {
      error("HasValues::save # Xerces exception: %s", XC(e.getMessage()));
      return false;
    }
  }

  // Dump the contents of the XML element (recursive)
  static void xml_recurse( DOMElement * e)
  {
    printf("<");
    DOMNode * n = e->getFirstChild();

    while(n) {

      DOMElement * e2 = dynamic_cast<DOMElement *> (n);
      DOMText    * t  = dynamic_cast<DOMText *> (n);

      const XMLCh * xstr1 = n->getNodeName();
      const XMLCh * xstr2 = n->getNodeValue();

      if(e2) {

	printf("DOM NODE [%s] [%s]\n",
	       XCStr(xstr1).str(), XCStr(xstr2).str());
	xml_recurse(e2);
      }
      if(t && !t->isIgnorableWhitespace()) {
	printf("DOM TEXT [%s]\n", XCStr(xstr2).str());
	
      }
      
      n = n->getNextSibling();
    }    
    printf(">");
  }


  void HasValues::init()
  {
    static bool once = false;

    if(once)
      return;

    once = true;

    try {
      /// @todo we should also call XMLPlatformUtils::Terminate()

      // Init Xerces
      XMLPlatformUtils::Initialize();
    }
    catch(...) {
      error("HasValues::init # Xerces init failed");
    }
    
  }

  bool HasValues::test(const char * filename)
  {
    const char * fname = "HasValues::test";

    init();

    trace("%s # XML init done", fname);

    TestOut outer1(false);
    TestOut outer2(true);

    trace("%s # test object created", fname);

    bool ok = outer1.save("foo-1.xml");
    ok = ok && outer2.save("foo-2.xml");
    
    trace("%s # test objects saved", fname);
    if(!outer2.load("foo-1.xml"))
      ok = false;
    if(!outer2.save("foo-4.xml"))
      ok = false;

    trace("%s # loaded/written", fname);

    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl =
      DOMImplementationRegistry::getDOMImplementation(gLS);
    DOMBuilder *parser = ((DOMImplementationLS*)impl)->createDOMBuilder
      (DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    parser->resetDocumentPool();
    
    DOMDocument * doc = parser->parseURI(filename);


    if(!doc) {
      error("%s # Could not load document %s", fname, filename);
    }
    else {
      DOMElement * elem = doc->getDocumentElement();
      xml_recurse(elem);
    }

    parser->release();
    
    XMLPlatformUtils::Terminate();

    return ok;
  }

  void HasValues::clearValues()
  {
    m_values.clear();
  }

  bool HasValues::readValue(DOMElement *)
  {
    return false;
  }

  void HasValues::addValueInternal(const char * id,  void * ptr, IO * io)
  {
    assert(id && ptr && io);
    m_values[id] = Ptr(ptr, io);
  }



}
