/* COPYRIGHT
 *
 * This file is part of MultiWidgets.
 *
 * Copyright: MultiTouch Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2007-2008
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from MultiTouch Oy.
 *
 * See file "MultiWidgets.hpp" for authors and more details.
 *
 */

#include "RenderContext.hpp"

#define DEFAULT_RECURSION_LIMIT 4

namespace Luminous
{
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

}

