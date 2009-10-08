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

  void RenderContext::drawLineRect(const Nimble::Rectf & r, float thickness, const float * rgba)
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


}

