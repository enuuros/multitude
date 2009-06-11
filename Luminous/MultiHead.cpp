/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "MultiHead.hpp"

#include "GLResources.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

#include <Radiant/Trace.hpp>

#include <Valuable/DOMElement.hpp>

#include <GL/glew.h>

namespace Luminous {

  using namespace Radiant;

  MultiHead::Area::Area()
    : HasValues(0, "Area"),
      m_keyStone(this, "keystone"),
      m_location(this, "location", Nimble::Vector2i(0, 0)),
      m_size(this, "size", Nimble::Vector2i(100, 100)),
      m_graphicsLocation(this, "graphicslocation", Nimble::Vector2i(0, 0)),
      m_graphicsSize(this, "graphicssize", Nimble::Vector2i(100, 100)),
      m_seams(this, "seams", Nimble::Vector4f(0, 0, 0, 0)),
      m_active(this, "active", 1),
      m_method(this, "method", METHOD_MATRIX_TRICK),
      m_comment(this, "comment"),
      m_graphicsBounds(0, 0, 100, 100),
      m_pixelSizeCm(0.1f)
  {
  }

  MultiHead::Area::~Area()
  {}

  bool MultiHead::Area::deserializeXML(Valuable::DOMElement element)
  {
    bool ok = HasValues::deserializeXML(element);

    updateBBox();

    return ok;
  }

  void MultiHead::Area::applyGlState() const
  {
    /* info("MultiHead::Area::applyGlState # %d %d %d %d",
       m_location[0], m_location[1], m_size[0], m_size[1]);
    */
    glViewport(m_location[0], m_location[1], m_size[0], m_size[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if(m_method == METHOD_MATRIX_TRICK)
      m_keyStone.applyGlState();

    glPushMatrix(); // Recovered in cleanEdges

    glOrtho(m_graphicsLocation[0] - m_seams[0],
	    m_graphicsLocation[0] + m_graphicsSize[0] + m_seams[1],
	    m_graphicsLocation[1] + m_graphicsSize[1] + m_seams[2], 
	    m_graphicsLocation[1] - m_seams[3], -1e3, 1e3);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void MultiHead::Area::cleanEdges() const
  {
    glViewport(m_location[0], m_location[1], m_size[0], m_size[1]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(); // From applyGlState
    glLoadIdentity();

    if(m_method == METHOD_TEXTURE_READBACK) {

      GLRESOURCE_ENSURE2(Texture2D, tex, this);

      if(tex->size() != m_size.asVector()) {
        info("Area GL init");
	// Initialize the texture to the right size:
	tex->loadBytes(GL_RGB, width(), height(), 0, 
		       Luminous::PixelFormat::rgbUByte(),
		       false);
      }

      tex->bind(GL_TEXTURE0);

      glReadBuffer(GL_BACK);
      glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		       m_location.asVector().x, m_location.asVector().y,
                       tex->width(), tex->height(), 0);

      glDisable(GL_TEXTURE_2D);
      glColor3f(0, 0, 0);
      gluOrtho2D(0, 1, 1, 0);
      Utils::glTexRect(0, 1, 1, 0);

      glLoadIdentity();
      m_keyStone.applyGlState();

      gluOrtho2D(0, 1, 1, 0);

      tex->bind(GL_TEXTURE0);
      glEnable(GL_TEXTURE_2D);

      glColor3f(1, 1, 1);
      Utils::glTexRect(-m_seams[0] / m_size[0], 1 + m_seams[2] / m_size[1],
		       1 + m_seams[1] / m_size[0], -m_seams[3] / m_size[1]);
    }
    else
      gluOrtho2D(0, 1, 1, 0);

    float gamma = 1.1f;

    if(m_seams[0] != 0.0f)
      Utils::fadeEdge(1, 1, 2 * m_seams[0] / m_size[0],
          gamma, Utils::LEFT, false);
    if(m_seams[1] != 0.0f)
      Utils::fadeEdge(1, 1, 2 * m_seams[1] / m_size[0],
          gamma, Utils::RIGHT, false);
    if(m_seams[2] != 0.0f)
      Utils::fadeEdge(1, 1, 2 * m_seams[2] / m_size[1],
          gamma, Utils::TOP, false);
    if(m_seams[3] != 0.0f)
      Utils::fadeEdge(1, 1, 2 * m_seams[3] / m_size[1],
          gamma, Utils::BOTTOM, false);

    // if(m_seams[2] != 0.0f || m_seams[3] != 0.0f)
    // puts("MultiHead::Area::cleanEdges # top & bottom unimplemented");

    if(m_method != METHOD_TEXTURE_READBACK) 
      m_keyStone.cleanExterior();
  }

  Nimble::Vector2f MultiHead::Area::windowToGraphics
  (Nimble::Vector2f loc, int windowheight, bool & isInside)
  {
    //      Radiant::trace("MultiHead::Area::windowToGraphics");

    assert((m_size[0] > 0.01f) && (m_size[1] > 0.01f));
    
    Nimble::Vector2f orig = loc;
    
    loc.x -= m_location[0];
    loc.y -= (windowheight - m_size[1] - m_location[1]);
    loc.descale(m_size.asVector());
    loc.y = 1.0f - loc.y;
    
    bool dontCare = false;
    Nimble::Matrix4 m = m_keyStone.matrix().inverse( &dontCare);
    assert(dontCare);
    
    loc = GLKeyStone::projectCorrected(m, loc).vector2();
    
    Nimble::Rectf rectangle(0.f, 0.f, 1.f, 1.f);    
    bool ok = rectangle.contains(loc);
    
    isInside = ok;
    
    loc.y = 1.0f - loc.y;
    loc.scale(m_graphicsSize.asVector());
    loc += m_graphicsLocation.asVector();
    
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
    : HasValues(0, "Window"),
    m_location(this, "location", Nimble::Vector2i(0, 0)),
    m_size(this, "size", Nimble::Vector2i(100, 100)),
    m_frameless(this, "frameless", 1),
    m_fullscreen(this, "fullscreen", 0),
    m_resizeable(this, "resizeable", 0),
    m_pixelSizeCm(0.1f)
  {
  }

  MultiHead::Window::~Window()
  {}

  void MultiHead::Window::resizeEvent(Vector2i size)
  {
    m_size = size;

    if(m_areas.size() == 1) {
      Radiant::debug("MultiHead::Window::resizeEvent");
      m_areas[0].ptr()->setSize(size);
    }
  }

  Nimble::Rect MultiHead::Window::graphicsBounds() const
  {
    if(!m_areas.size())
      return Nimble::Rect(0,0, 1, 1);

    Rect r = m_areas[0].ptr()->graphicsBounds();

    for(unsigned i = 1; i < m_areas.size(); i++) {
      r.expand(m_areas[i].ptr()->graphicsBounds());
    }
  
    return r;
  }

  void MultiHead::Window::setSeam(float seam)
  {
    for(unsigned i = 0; i < m_areas.size(); i++) {
      m_areas[i].ptr()->setSeams(i == 0 ? 0 : seam,
          i + 1 >= m_areas.size() ? 0 : seam,
          0, 0);
    }
  }

  Nimble::Vector2f MultiHead::Window::windowToGraphics(Nimble::Vector2f loc, bool & convOk)
    {
//      Radiant::trace("MultiHead::Window::windowToGraphics # loc(%f,%f), m_size[1] = %d", loc.x, loc.y, m_size[1]);

      for(unsigned i = 0; i < m_areas.size(); i++) {
        bool ok = false;
        Nimble::Vector2f res = m_areas[i].ptr()->windowToGraphics(loc, m_size[1], ok);

        if(ok) {
            convOk = true;
          return res;
        }
      }
      
      convOk = false;

      return Nimble::Vector2f(0, 0);
    }

  void MultiHead::Window::setPixelSizeCm(float sizeCm)
  {
    assert(sizeCm > 0.0f);

    m_pixelSizeCm = sizeCm;

    for(unsigned i = 0; i < m_areas.size(); i++)
      m_areas[i].ptr()->setPixelSizeCm(sizeCm);
  }

  bool MultiHead::Window::readElement(Valuable::DOMElement ce)
  {
    const std::string & name = ce.getTagName();

    // Get the 'type' attribute
    if(!ce.hasAttribute("type")) {
      Radiant::error("MultiHead::Window::readElement # "
		     "no type attribute on element '%s'", name.c_str());
      return false;
    }

    const std::string & type = ce.getAttribute("type");

    if(type == std::string("area")) {
      Area * area = new Area();
      // Add as child & recurse
      addValue(name, area);
      area->deserializeXML(ce);
      m_areas.push_back(area);
    } else {
      return false;
    }

    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  MultiHead::MultiHead()
  : HasValues(0, "MultiHead", false),
    m_widthcm(this, "widthcm", 100, true),
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

  MultiHead::Window & MultiHead::window(unsigned i)
  {
    if(i >= m_windows.size()) {
      Radiant::fatal("MultiHead::window # Array index %u exceeds array size %u",
		     i, (unsigned) m_windows.size());
    }

    return * m_windows[i].ptr();
  }

  const MultiHead::Window & MultiHead::window(unsigned i) const
  {
    if(i >= m_windows.size()) {
      Radiant::fatal("MultiHead::window # Array index %u exceeds array size %u",
		     i, (unsigned) m_windows.size());
    }

    return * m_windows[i].ptr();
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

  Nimble::Vector2i MultiHead::totalSize()
  {
    if(!windowCount())
      return Nimble::Vector2i(0, 0);

    Window * w = & window(0);

    Nimble::Vector2i low = w->location();
    Nimble::Vector2i high = w->location() + w->size();
    
    for(unsigned i = 0; i < windowCount(); i++) {

      w = & window(i);

      Nimble::Vector2i low2 = w->location();
      Nimble::Vector2i high2 = w->location() + w->size();

      low.x = Nimble::Math::Min(low.x, low2.x);
      low.y = Nimble::Math::Min(low.y, low2.y);

      high.x = Nimble::Math::Max(high.x, high2.x);
      high.y = Nimble::Math::Max(high.y, high2.y);
    }

    return high - low;
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
    float left =  1000000;
    float right =-1000000;

    int n = areaCount();

    Radiant::debug("MultiHead::width # %d", n);

    for(int i = 0; i < n; i++) {
      Area & a = area(i);

      if(!a.active())
        continue;

      float wleft  = a.graphicsLocation().x;
      float wright = wleft + a.graphicsSize().x;

      left  = Nimble::Math::Min(left,  wleft);
      right = Nimble::Math::Max(right, wright);
      
      Radiant::debug("lr = %f %f", left, right);
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

  bool MultiHead::deserializeXML(Valuable::DOMElement element)
  {
    m_windows.clear();

    bool ok = HasValues::deserializeXML(element);

    const float pixelSizeCm = m_widthcm.asFloat() / width();

    for(unsigned i = 0; i < windowCount(); i++) {
      window(i).setPixelSizeCm(pixelSizeCm);      
    }

    m_edited = false;

    return ok;
  }

  bool MultiHead::readElement(Valuable::DOMElement ce)
  {
    const std::string & name = ce.getTagName();

    // Get the 'type' attribute
    if(!ce.hasAttribute("type")) {
      Radiant::error("MultiHead::readElement # no type attribute on element '%s'", name.c_str());
      return false;
    }

    const std::string & type = ce.getAttribute("type");

    if(type == std::string("window")) {
      Window * win = new Window();

      // Add as child & recurse
      addValue(name, win);
      win->deserializeXML(ce);


      m_windows.push_back(win);
    } else {
      return false;
    }

    return true;
  }

}
