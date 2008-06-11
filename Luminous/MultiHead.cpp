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

#include <GL/glew.h>

namespace Luminous {

  using Radiant::error;
  using Radiant::trace;

  MultiHead::Area::Area()
    : HasValues(0, "Area", false),
    m_location(this, "location", false, Nimble::Vector2i(0, 0)),
    m_size(this, "size", false, Nimble::Vector2i(100, 100)),
    m_graphicsLocation(this, "graphicslocation", false, Nimble::Vector2i(0, 0)),
    m_graphicsSize(this, "graphicssize", false, Nimble::Vector2i(100, 100)),
    m_seams(this, "seams", false, Nimble::Vector4f(0, 0, 0, 0)),
    m_active(this, "active", false, 1),
    m_comment(this, "comment", false, ""),
    m_graphicsBounds(0, 0, 100, 100),
    m_pixelSizeCm(0.1f)
  {
    addValue("keystone", &m_keyStone);
  }

  MultiHead::Area::~Area()
  {}

  bool MultiHead::Area::deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & cl)
  {
    bool ok = HasValues::deserializeXML(element, cl);

    updateBBox();

    return ok;
  }

  void MultiHead::Area::applyGlState()
  {
    // printf("MultiHead::Area::applyGlState # %d %d\n",
    // m_location.x, m_location.y);
    glViewport(m_location[0], m_location[1], m_size[0], m_size[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_keyStone.applyGlState();
    glPushMatrix(); // Recovered in cleanEdges
/*
    gluOrtho2D(m_graphicsLocation[0] - m_seams[0],
        m_graphicsLocation[0] + m_graphicsSize[0] + m_seams[1],
        m_graphicsLocation[1] + m_graphicsSize[1] + m_seams[2], 
        m_graphicsLocation[1] - m_seams[3]);
*/
    glOrtho(m_graphicsLocation[0] - m_seams[0],
        m_graphicsLocation[0] + m_graphicsSize[0] + m_seams[1],
        m_graphicsLocation[1] + m_graphicsSize[1] + m_seams[2], 
        m_graphicsLocation[1] - m_seams[3], -1e3, 1e3);


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
      Utils::fadeEdge(1, 1, 1.62f * m_seams[0] / m_size[0],
          gamma, Utils::LEFT, false);
    if(m_seams[1] != 0.0f)
      Utils::fadeEdge(1, 1, 1.62f * m_seams[1] / m_size[0],
          gamma, Utils::RIGHT, false);

    if(m_seams[2] != 0.0f || m_seams[3] != 0.0f)
      puts("MultiHead::Area::cleanEdges # top & bottom unimplemented");

    m_keyStone.cleanExterior();
  }

  Nimble::Vector2f MultiHead::Area::windowToGraphics
    (Nimble::Vector2f loc, int windowheight, bool * convOk)
    {
      assert((m_size[0] > 0.01f) && (m_size[1] > 0.01f));

      Nimble::Vector2f orig = loc;

      loc.x -= m_location[0];
      loc.y -= (windowheight - m_size[1] - m_location[1]);
      loc.descale(m_size.asVector());
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
      loc.scale(m_graphicsSize.asVector());
      loc += m_graphicsLocation.asVector();

      /// @todo commenting this breaks things on single display machines (where's the bug?)    
//      trace("MultiHead::Area::windowToGraphics # %.2f %.2f to %.2f %.2f %d (%s)",
//          orig.x, orig.y, loc.x, loc.y, (int) foo, ok ? "ok" : "fail");

      return loc;
    }

  void MultiHead::Area::updateBBox()
  {
    m_graphicsBounds.set
      (m_graphicsLocation.asVector(),
       m_graphicsLocation.asVector() + m_graphicsSize.asVector());
    m_graphicsBounds.low().x  -= m_seams[0];
    m_graphicsBounds.high().x += m_seams[1];
    m_graphicsBounds.low().y  -= m_seams[2];
    m_graphicsBounds.high().y += m_seams[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  MultiHead::Window::Window()
    : m_location(this, "location", false, Nimble::Vector2i(0, 0)),
    m_size(this, "size", false, Nimble::Vector2i(100, 100)),
    m_frameless(this, "frameless", false, 1),
    m_fullscreen(this, "fullscreen", false, 0),
    m_resizeable(this, "resizeable", false, 0),
    m_pixelSizeCm(0.1f)
  {
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

  Nimble::Vector2f MultiHead::Window::windowToGraphics
    (Nimble::Vector2f loc, bool * convOk)
    {
      for(uint i = 0; i < m_areas.size(); i++) {
        bool ok = false;
        Nimble::Vector2f res =
          m_areas[i].ptr()->windowToGraphics(loc, m_size[1], & ok);

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

  bool MultiHead::Window::readElement(xercesc::DOMElement * ce,
				      CL::ClassLoader<ValueObject> & cl)
  {
    using namespace xercesc;

    char * nameVal = XMLString::transcode(ce->getTagName());
    XMLCh * typeAttr = XMLString::transcode("type");

    // Get the 'type' attribute
    if(!ce->hasAttribute(typeAttr)) {
      Radiant::error("MultiHead::Window::readElement # "
		     "no type attribute on element '%s'", nameVal);
      XMLString::release(&nameVal);
      XMLString::release(&typeAttr);
      return false;
    }

    const XMLCh * typeVal = ce->getAttribute(typeAttr);
    char * myType = XMLString::transcode(typeVal);

    if(strcmp(myType, "area") == 0) {
      Area * area = new Area();
      // Add as child & recurse
      addValue(nameVal, area);
      area->deserializeXML(ce, cl);
      m_areas.push_back(area);
    } else {
      XMLString::release(&myType);
      return false;
    }

    XMLString::release(&myType);
    XMLString::release(&nameVal);
    XMLString::release(&typeAttr);
    
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  MultiHead::MultiHead()
  : HasValues(0, "MultiHead", false),
    m_widthcm(this, "widthcm", false, 100.f),
    m_edited(false)
  {}

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
    float left = 1000000;
    float right = -1000000;

    int n = areaCount();

    for(int i = 0; i < n; i++) {
      Area & a = area(i);

      if(!a.active())
        continue;

      float wleft  = a.graphicsLocation().x;
      float wright = wleft + a.graphicsSize().x;

      left  = Nimble::Math::Min(left,  wleft);
      right = Nimble::Math::Max(right, wright);
    }

    return (int) (right - left);
  }

  int MultiHead::height()
  {
    float top = 1000000;
    float bottom = -1000000;

    int n = areaCount();

    for(int i = 0; i < n; i++) {
      Area & a = area(i);

      float wtop = a.graphicsLocation().y;
      float wbot = wtop + a.graphicsSize().y;

      top = Nimble::Math::Min(top, wtop);
      bottom = Nimble::Math::Max(bottom, wbot);
    }

    return (int) (bottom - top);
  }

  bool MultiHead::deserializeXML(xercesc::DOMElement * element,
                                 CL::ClassLoader<ValueObject> & cl)
  {
    m_windows.clear();

    bool ok = HasValues::deserializeXML(element, cl);

    m_edited = false;

    return ok;
  }

  bool MultiHead::readElement(xercesc::DOMElement * ce, CL::ClassLoader<ValueObject> & cl)
  {
    using namespace xercesc;

    char * nameVal = XMLString::transcode(ce->getTagName());
    XMLCh * typeAttr = XMLString::transcode("type");

    // Get the 'type' attribute
    if(!ce->hasAttribute(typeAttr)) {
      Radiant::error("MultiHead::readElement # no type attribute on element '%s'", nameVal);
      XMLString::release(&nameVal);
      XMLString::release(&typeAttr);
      return false;
    }

    const XMLCh * typeVal = ce->getAttribute(typeAttr);
    char * myType = XMLString::transcode(typeVal);

    if(strcmp(myType, "window") == 0) {
      Window * win = new Window();

      // Add as child & recurse
      addValue(nameVal, win);
      win->deserializeXML(ce, cl);

      const float pixelSizeCm = m_widthcm.asFloat() / width();
      win->setPixelSizeCm(pixelSizeCm);

      m_windows.push_back(win);
    } else {
      XMLString::release(&myType);
      return false;
    }

    XMLString::release(&myType);
    XMLString::release(&nameVal);
    XMLString::release(&typeAttr);
    
    return true;
  }

}
