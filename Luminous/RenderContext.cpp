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

