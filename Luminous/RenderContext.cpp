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

#include "RenderContext.hpp"

#include "Utils.hpp"

#define DEFAULT_RECURSION_LIMIT 4

namespace Luminous
{

  using namespace Nimble;

  RenderContext::RenderContext(Luminous::GLResources * resources)
  : Transformer(),
    m_resources(resources),
    m_recursionLimit(DEFAULT_RECURSION_LIMIT),
    m_recursionDepth(0)
  {
    prepare();
  }

  RenderContext::~RenderContext()
  {
  }

  void RenderContext::prepare()
  {
    resetTransform();
    m_recursionDepth = 0;

    // Make sure the clip stack is empty
    while(!m_clipStack.empty())
      m_clipStack.pop();
  } 

  void RenderContext::finish()
  {}

  void RenderContext::pushClipRect(const Nimble::Rect & area)
  {
    m_clipStack.push(area);
  }

  void RenderContext::popClipRect()
  {
    m_clipStack.pop();
  } 

  bool RenderContext::isVisible(const Nimble::Rect & area)
  {
    if(m_clipStack.empty()) 
      return true;
    else 
      return m_clipStack.top().intersects(area);
  }

  const Nimble::Rect & RenderContext::visibleArea() const
  {
    return m_clipStack.top();
  }

  void RenderContext::drawLineRect(const Nimble::Rectf & r,
                                   float thickness, const float * rgba)
  {
    thickness *= 0.5f;

    Vector2 v1(thickness, thickness);

    Nimble::Rectf inside(r.low() + v1, r.high() - v1);
    Nimble::Rectf outside(r.low() - v1, r.high() + v1);

    Utils::glRectWithHoleAA(outside, inside, transform(), rgba);
  }


  void RenderContext::drawRect(const Nimble::Rectf & rect, const float * rgba)
  {
    Utils::glTexRectAA(rect.size(), 
		       transform() * Matrix3::translate2D(rect.low()), rgba);
  }
  
  
  void RenderContext::drawCircle(Nimble::Vector2f center, float radius, 
                                 const float * rgba, int segments)
  {
    if(segments < 0) {
      float realRad = radius * transform().extractScale();
      segments = Math::Clamp((int) realRad * 2, 6, 60);
    }
    
    Nimble::Matrix3 m(transform() * Nimble::Matrix3::translate2D(center));

    Utils::glSolidSoftCircle(m, radius, 1.0f, segments, rgba);
  }

  void RenderContext::drawPolyLine(const Nimble::Vector2f * vertices, int n,
				   float width, const float * rgba)
  {
    if(n < 2)
      return;

    float r = rgba[0];
    float g = rgba[1];
    float b = rgba[2];
    float a = rgba[3];

    width *= scale() * 0.5f;
    float fullw = width + 1;
    
    Matrix3 m(transform());

    Vector2f cprev = m.project(vertices[0]);

    Vector2f dir0 = m.project(vertices[1]) - cprev;

    Vector2 p01 = dir0.perpendicular();
    p01.normalize();

    for(int i = 1; i < n; i++) {

      Vector2f cnow  = m.project(vertices[i]);

      if((cnow - cprev).length() < 3.0f && i != (n-1))
        continue;
      
      Vector2f cnext;
      
      if(i < (n - 1))
        cnext = m.project(vertices[i + 1]);
      else
        cnext = 2.0f * cnow - cprev;

      Vector2f dir1 = cnext - cnow;
      Vector2f dir2 = cnow - cprev;

      dir1.normalize();
      dir2.normalize();

      Vector2 q = dir1.perpendicular();

      Vector2 p12 = (dir2 + dir1).perpendicular();
      p12.normalize();

      float q12 = dot(p12, q);
      if(q12 > 0.1f)
        p12 /= q12;

      glBegin(GL_QUAD_STRIP);
      
      glColor4f(r, g, b, 0.0f);
      glVertex2fv((cprev + p01 * fullw).data());
      glVertex2fv((cnow + p12 * fullw).data());

      glColor4f(r, g, b, a);
      glVertex2fv((cprev + p01 * width).data());
      glVertex2fv((cnow + p12 * width).data());

      glVertex2fv((cprev - p01 * width).data());
      glVertex2fv((cnow - p12 * width).data());

      glColor4f(r, g, b, 0.0f);
      glVertex2fv((cprev - p01 * fullw).data());
      glVertex2fv((cnow - p12 * fullw).data());

      glEnd();

      p01 = p12;

      cprev = cnow;
    }

  }

  void RenderContext::drawTexRect(Nimble::Vector2 size, const float * rgba)
  {
    Nimble::Matrix3 m = transform();

    const Vector4 v[4] = {
      Utils::project(m, Vector2(0,       0)),
      Utils::project(m, Vector2(size.x,  0)),
      Utils::project(m, Vector2(size.x,  size.y)),
      Utils::project(m, Vector2(0,       size.y))
    };

    if(rgba)
      glColor4fv(rgba);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex4fv(v[0].data());

    glTexCoord2f(1.0f, 0.0f);
    glVertex4fv(v[1].data());
  
    glTexCoord2f(1.0f, 1.0f);
    glVertex4fv(v[2].data());
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex4fv(v[3].data());

    glEnd();

  }

  void RenderContext::setBlendFunc(BlendFunc f)
  {
    if(f == BLEND_NONE) {
      glDisable(GL_BLEND);
      return;
    }

    glEnable(GL_BLEND);

    if(f == BLEND_USUAL)
      Utils::glUsualBlend();
    else if(f == BLEND_ADDITIVE)
      Utils::glAdditiveBlend();
    else if(f == BLEND_SUBTRACTIVE) {
      Utils::glSubtractiveBlend();
    }

  }

  const char ** RenderContext::blendFuncNames()
  {
    static const char * names [] = {
      "usual",
      "none",
      "additive",
      "subtractive"
    };

    return names;
  }

}

