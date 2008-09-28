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


#ifndef LUMINOUS_GL_KEYSTONE_HPP
#define LUMINOUS_GL_KEYSTONE_HPP

#include <Nimble/Matrix4.hpp>

#include <Valuable/HasValues.hpp>
#include <Valuable/ValueInt.hpp>
#include <Valuable/ValueVector.hpp>

namespace Luminous {

  /** Class for doing key-stone correction when rendering 2D OpenGL
      graphics. */
  class GLKeyStone : public Valuable::HasValues
  {
  public:
    GLKeyStone(Valuable::HasValues * parent, const std::string & name);
    virtual ~GLKeyStone();

    virtual const char * const type() const { return "glkeystone"; }

    virtual bool deserializeXML(Valuable::DOMElement e);

    int closestVertex(Nimble::Vector2 loc);
    void setVertex(int index, float x, float y) 
    { m_vertices[index] = Nimble::Vector2f(x, y); }

    bool moveVertex(Nimble::Vector2 loc);
    void selectVertex(Nimble::Vector2 loc);
    void selectNextVertex()
    { m_lastMove = (m_lastMove + 1) % 4; }

    void moveLastVertex(const Nimble::Vector2 & move)
    { m_vertices[m_lastMove] += move; calculateMatrix(); }

    int lastMove() const { return m_lastMove; }
    Nimble::Vector2f lastMoveVertex() const
    { return m_vertices[m_lastMove].asVector(); }

    int rotations() const { return m_rotations.asInt(); }

    void rotateVertices();

    void calculateMatrix();
    
    const Nimble::Matrix4 & matrix() const { return m_matrix; }
    /** Projects the vector v using internal matrix, WITHOUT applying
	perspective correction. */
    Nimble::Vector4 project(Nimble::Vector2 v);
    /** Projects the vector v using matrix m, applying perspective
	correction. */
    static Nimble::Vector4 projectCorrected(const Nimble::Matrix4 & m,
					    Nimble::Vector2 v);
    
    void applyGlState();

    void cleanExterior();

    Nimble::Vector2 closest(Nimble::Vector2 v) const;

  protected:
//    Valuable::ValueVector2f m_vertices[4];
    Valuable::ValueVector2f m_vertices[4];    
    Nimble::Matrix4 m_matrix;
    int     m_lastMove;
    Valuable::ValueInt m_rotations;
  };

}

#endif
