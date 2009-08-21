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
#ifndef LUMINOUS_TRANSFORMER_HPP
#define LUMINOUS_TRANSFORMER_HPP

#include <Luminous/Export.hpp>

#include <Nimble/Matrix3.hpp>

#include <stack>

namespace Luminous
{
  /** Geometrical 2D transformation stack. This class encapsulates 2D
      transformation stack. The transformations are stored as 3x3
      matrices. */
  class LUMINOUS_API Transformer
  {
  public:
    Transformer();
    ~Transformer();

    /// Get the top matrix of the stack
    const Nimble::Matrix3 & transform() const { return m_stack.top(); }

    Nimble::Vector2 project(Nimble::Vector2);

    /// Pops the top matrix from the stack
    void popTransform() { m_stack.pop(); }
   
    /// Multiply the top matrix from the left with the given matrix and push the
    /// result into the stack
    void pushTransformLeftMul(const Nimble::Matrix3 & m);
    /// Multiply the top matrix from the right with the given matrix and push
    /// the result into the stack
    void pushTransformRightMul(const Nimble::Matrix3 & m);

    /// Clears the stack so it only contains an identity matrix
    void resetTransform();

  protected:
    std::stack<Nimble::Matrix3> m_stack;
  };
}

#endif

