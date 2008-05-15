/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/MultiHead.hpp>
#include <Luminous/Utils.hpp>
#include <Radiant/Trace.hpp>
#include <ValueIO/Dom.hpp>

#include <GL/glew.h>

namespace Luminous {

  using Radiant::error;
  using Radiant::trace;

  MultiHead::Area::Area()
    : m_location(0, 0),
      m_size(100, 100),
      m_graphicsLocation(0, 0),
      m_graphicsSize(100, 100),
      m_seams(0, 0, 0, 0),
      m_active(1),
      m_graphicsBounds(0, 0, 100, 100),
      m_pixelSizeCm(0.1f)
  {
    addValue("active",   m_active);
    addValue("comment",  & m_comment);
    addValue("keystone", & m_keyStone);
    addValue("location", m_location);
    addValue("graphicslocation", m_graphicsLocation);
    addValue("graphicssize", m_graphicsSize);
    addValue("seams",    m_seams);
    addValue("size",     m_size);
  }

  MultiHead::Area::~Area()
  {}

  bool MultiHead::Area::readDom(DOMElement * elem)
  {
    bool ok = HasValues::readDom(elem);

    updateBBox();

    return ok;
  }

  void MultiHead::Area::applyGlState()
  {
    // printf("MultiHead::Area::applyGlState # %d %d\n",
    // m_location.x, m_location.y);
    glViewport(m_location.x, m_location.y, m_size.x, m_size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_keyStone.applyGlState();
    glPushMatrix(); // Recovered in cleanEdges
    gluOrtho2D(m_graphicsLocation.x - m_seams[0],
	       m_graphicsLocation.x + m_graphicsSize.x + m_seams[1],
	       m_graphicsLocation.y + m_graphicsSize.y + m_seams[2], 
	       m_graphicsLocation.y - m_seams[3]);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
  
  void MultiHead::Area::cleanEdges()
  {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // From applyGlState

    gluOrtho2D(0, 1, 1, 0);
    
    float gamma = 1.35f;

    if(m_seams[0] != 0.0f)
      Utils::fadeEdge(1, 1, 1.62f * m_seams[0] / m_size.x,
                      gamma, Utils::LEFT, false);
    if(m_seams[1] != 0.0f)
      Utils::fadeEdge(1, 1, 1.62f * m_seams[1] / m_size.x,
                      gamma, Utils::RIGHT, false);

    if(m_seams[2] != 0.0f || m_seams[3] != 0.0f)
      puts("MultiHead::Area::cleanEdges # top & bottom unimplemented");

    m_keyStone.cleanExterior();
  }

  Nimble::Vector2f MultiHead::Area::windowToGraphics
  (Nimble::Vector2f loc, int windowheight, bool * convOk)
  {
    assert((m_size.x > 0.01f) && (m_size.y > 0.01f));

    Nimble::Vector2f orig = loc;

    loc.x -= m_location.x;
    loc.y -= (windowheight - m_size.y - m_location.y);
    loc.descale(m_size);
    loc.y = 1.0f - loc.y;

    bool foo = true;
    Nimble::Matrix4 m = m_keyStone.matrix().inverse( & foo);

    loc = GlKeyStone::projectCorrected(m, loc).vector2();

    Nimble::Rect area(0, 0, 1, 1);    
    bool ok = area.contains(loc);
    if(convOk) {
      *convOk = ok;
    }

    loc.y = 1.0f - loc.y;
    loc.scale(m_graphicsSize);
    loc += m_graphicsLocation;

    /// @todo commenting this breaks things on single display machines (where's the bug?)    
    trace("MultiHead::Area::windowToGraphics # %.2f %.2f to %.2f %.2f %d (%s)",
    orig.x, orig.y, loc.x, loc.y, (int) foo, ok ? "ok" : "fail");

    return loc;
  }

  void MultiHead::Area::updateBBox()
  {
    m_graphicsBounds.set
      (m_graphicsLocation, m_graphicsLocation + m_graphicsSize);
    m_graphicsBounds.low().x  -= m_seams[0];
    m_graphicsBounds.high().x += m_seams[1];
    m_graphicsBounds.low().y  -= m_seams[2];
    m_graphicsBounds.high().y += m_seams[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  MultiHead::Window::Window()
    : m_location(0, 0),
      m_size(100, 100),
      m_frameless(1),
      m_fullscreen(0),
      m_resizeable(0),
      m_pixelSizeCm(0.1f)
  {
    addValue("frameless", m_frameless);
    addValue("fullscreen", m_fullscreen);
    addValue("resizeable", m_resizeable);
    addValue("location", m_location);
    addValue("size",     m_size);
  }

  MultiHead::Window::~Window()
  {}

  void MultiHead::Window::resizeEvent(Vector2i size)
  {
    m_size = size;

    if(m_areas.size() == 1) {
      trace("MultiHead::Window::resizeEvent");
      m_areas[0].ptr()->setSize(size);
    }
  }

  void MultiHead::Window::setSeam(float seam)
  {
    for(unsigned i = 0; i < m_areas.size(); i++) {
      m_areas[i].ptr()->setSeams(i == 0 ? 0 : seam,
				 i + 1 >= m_areas.size() ? 0 : seam,
				 0, 0);
    }
  }

  DOMElement * MultiHead::Window::writeDom(DOMDocument * doc)
  {
    DOMElement * e = ValueIO::HasValues::writeDom(doc);

    for(unsigned i = 0; i < m_areas.size(); i++) {
      DOMElement * si = doc->createElement(XW("area"));
      si->appendChild(m_areas[i].ptr()->writeDom(doc));
      e->appendChild(si);
    }
    
    return e;
  }

  bool MultiHead::Window::readDom(DOMElement * e)
  {
    m_areas.clear();
    bool ok = ValueIO::HasValues::readDom(e);

    return ok && m_areas.size() != 0;
  }
  
  Nimble::Vector2f MultiHead::Window::windowToGraphics
  (Nimble::Vector2f loc, bool * convOk)
  {
    for(uint i = 0; i < m_areas.size(); i++) {
      bool ok = false;
      Nimble::Vector2f res =
	m_areas[i].ptr()->windowToGraphics(loc, m_size.y, & ok);

      if(ok) {
	// puts("CONV OK");
	if(convOk)
	  *convOk = true;
	return res;
      }
    }

    if(convOk)
      *convOk = false;

    return Nimble::Vector2f(0, 0);
  }

  void MultiHead::Window::setPixelSizeCm(float sizeCm)
  {
    m_pixelSizeCm = sizeCm;

    for(uint i = 0; i < m_areas.size(); i++)
      m_areas[i].ptr()->setPixelSizeCm(sizeCm);
  }

  bool MultiHead::Window::readValue(DOMElement * e)
  {
    if(XMLString::equals(e->getNodeName(), XW("area"))) {
      Area * a = new Area();
      if(!a->readDom(e)) {
	error("MultiHead::Window::readValue # Area::readDom failed");
	delete a;
	return false;
      }
      else {
	m_areas.push_back(a);
	return true;
      }
    }
    
    return false;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  
  MultiHead::MultiHead()
    : m_widthcm(100)
  {
    setName("multihead");
    addValue("widthcm", m_widthcm);
  }

  MultiHead::~MultiHead()
  {}

  void MultiHead::makeSingle(int x, int y, int w, int h)
  {
    m_windows.clear();

    Area * a = new Area();
    a->setGeometry(0, 0, w, h);
    
    Window * wi = new Window();
    wi->setGeometry(x, y, w, h);

    wi->addArea(a);

    m_windows.push_back(wi);
  }

  void MultiHead::makeDouble(int x, int y, int w, int h, float seam)
  {
    m_windows.clear();

    int w2 = w / 2;

    Area * a1 = new Area();
    a1->setGeometry(0, 0, w2, h);
    a1->setSeams(0, seam, 0, 0);

    Area * a2 = new Area();
    a2->setGeometry(w2, 0, w2, h);
    a2->setSeams(seam, 0, 0, 0);

    Window * wi = new Window();
    wi->setGeometry(x, y, w, h);

    wi->addArea(a1);
    wi->addArea(a2);

    m_windows.push_back(wi);    
  }

  void MultiHead::makeQuadSideways
  (int x, int y, int w, int h, float seam)
  {
    m_windows.clear();

    int w4 = w / 4;

    int visibleWidth = h;
    int visibleHeight = w4;

    Window * wi = new Window();
    wi->setGeometry(x, y, w, h);

    for(int i = 0; i < 4; i++) {
      Area * a = new Area();
      a->setGeometry(i * w4, 0, w4, h, false);
      a->setGraphicsGeometry(i * visibleWidth, 0,
			     visibleWidth, visibleHeight);
      a->keyStone().rotateVertices();
      a->keyStone().rotateVertices();
      a->keyStone().rotateVertices();
      // a->keyStone().rotateVertices();
      a->setSeams(i == 0 ? 0 : seam, i == 3 ? 0 : seam, 0, 0);

      wi->addArea(a);
    }

    m_windows.push_back(wi);    
  }

  unsigned MultiHead::areaCount()
  {
    unsigned n = 0;

    for(unsigned i = 0; i < m_windows.size(); i++)
      n += m_windows[i].ptr()->areaCount();
    
    return n;
  }

  MultiHead::Area & MultiHead::area(unsigned index, MultiHead::Window ** winptr)
  {
    unsigned used = 0;
    
    for(unsigned i = 0; i < m_windows.size(); i++) {
      int n = m_windows[i].ptr()->areaCount();
      if(used + n > index) {
	if(winptr)
	  *winptr = m_windows[i].ptr();
	return m_windows[i].ptr()->area(index - used);
      }
      used += n;
    }

    assert(false); // Out of range

    return m_windows[0].ptr()->area(0); // Unreachable
  }

  float MultiHead::seam()
  {
    assert(areaCount() > 0);

    return area(0).maxSeam();
  }
   
  void MultiHead::setSeam(float seam)
  {
    assert(areaCount() > 0);

    for(unsigned i = 0; i < m_windows.size(); i++)
      m_windows[i].ptr()->setSeam(seam);    
  }

  int MultiHead::width()
  {
    int left = 1000000;
    int right = -1000000;
    
    int n = areaCount();
    
    for(int i = 0; i < n; i++) {
      Area & a = area(i);

      if(!a.active())
        continue;

      int wleft  = a.graphicsLocation().x;
      int wright = wleft + a.graphicsSize().x;

      left  = Nimble::Math::Min(left,  wleft);
      right = Nimble::Math::Max(right, wright);
    }

    return right - left;
  }

  int MultiHead::height()
  {
    int top = 1000000;
    int bottom = -1000000;

    int n = areaCount();
    
    for(int i = 0; i < n; i++) {
      Area & a = area(i);

      int wtop = a.graphicsLocation().y;
      int wbot = wtop + a.graphicsSize().y;
      
      top = Nimble::Math::Min(top, wtop);
      bottom = Nimble::Math::Max(bottom, wbot);
    }
    
    return bottom - top;
  }

  DOMElement * MultiHead::writeDom(DOMDocument * doc)
  {
    DOMElement * e = ValueIO::HasValues::writeDom(doc);

    for(unsigned i = 0; i < m_windows.size(); i++) {
      DOMElement * si = doc->createElement(XW("window"));
      si->appendChild(m_windows[i].ptr()->writeDom(doc));
      e->appendChild(si);
    }
    
    return e;
  }

  bool MultiHead::readDom(DOMElement * e)
  {
    HasValues::readDom(e);

    DOMNode * n = e->getFirstChild();

    //printf("MultiHead::readDom # %p %p\n", e, n);

    if(!n)
      return false;

    ValueIO::XCStr tag1(n->getNodeName());

    while(strcmp(tag1.str(), "values") != 0 && n) {
      n = n->getNextSibling();
      if(n)
	tag1 = n->getNodeName();
    }

    if(!n)
      return false;

    n = n->getFirstChild();

    bool ok = true;

    m_windows.clear();

    while (n) {
      ValueIO::XCStr tag(n->getNodeName());

      //printf("Found tag \"%s\"\n", tag.str());

      DOMElement * n2 = dynamic_cast<DOMElement *> (n);
      
      if(n2 && strcmp(tag.str(), "window") == 0) {
	Window * wi = new Window();
	if(!wi->readDom(n2)) {
	  error("MultiHead::readDom # Window::readDom failed");
	  delete wi;
	  ok = false;
	}
	else
	  m_windows.push_back(wi);
      }
      n = n->getNextSibling();
    }

    float pixelSizeCm = m_widthcm / width();

    for(uint i = 0; i < m_windows.size(); i++) {
      m_windows[i]->setPixelSizeCm(pixelSizeCm);
    }

    return ok && m_windows.size();
  }

  void MultiHead::test()
  {
    ValueIO::HasValues::init();

    MultiHead mh;

    mh.makeSingle(1280, 0, 1280, 768);
    mh.save("multihead-1.xml");

    // mh.makeDouble(0, 0, 2048, 768, 50);
    mh.makeDouble(500, 50, 1000, 500, 50);
    mh.save("multihead-2.xml");

    MultiHead mh2;

    bool ok = mh2.load("multihead-1.xml");

    trace(ok ?
	  "multihead-1.xml loaded ok" : "multihead-1.xml loading failed");

    
    mh2.save("multihead-1-b.xml");
  }


}
