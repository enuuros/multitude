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
#include "Transformer.hpp"

namespace Luminous 
{

  Transformer::Transformer()
  {
    resetTransform();
  }

  Transformer::~Transformer()
  {}

  void Transformer::resetTransform()
  {
    while(!m_stack.empty())
      m_stack.pop();

    Nimble::Matrix3 m;
    m.identity();

    m_stack.push(m);
  }

  void Transformer::pushTransformLeftMul(const Nimble::Matrix3 & m)
  {
    m_stack.push(m * m_stack.top());
  }

  void Transformer::pushTransformRightMul(const Nimble::Matrix3 & m)
  {
    m_stack.push(m_stack.top() * m);
  }

}
