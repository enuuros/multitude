#ifndef NIMBLE_RECTANGLE_HPP
#define NIMBLE_RECTANGLE_HPP

#include <Nimble/Export.hpp>
#include <Nimble/Vector2.hpp>

namespace Nimble {

  /** A rectangle is defined by origin (O), two unit-length axis vectors
  (U, V), and two non-negative extents (e1, e2). Point P=O+x*U+y*V
  is inside or on the rectangle whenever |x|<=e1 and |y|<=e2.*/
  class NIMBLE_API Rectangle
  {
    public:
    /// Default constructor (does not initialize values)
    Rectangle();
      /// Construct a new rectangle
      /// @param origin center of the box
      /// @param a0 unit-length axis vector
      /// @param e0 non-negative extent along a0
      /// @param a1 unit-length axis vector
      /// @param e1 non-negative extent along a1
      Rectangle(Nimble::Vector2f origin, Nimble::Vector2f a0, float e0, Nimble::Vector2f a1, float e1);

      /// Test if a point is inside the rectangle
      /// @param p point to test
      /// @return true if p is inside or on the rectangle
      bool inside(Nimble::Vector2f p) const;

    private:
      Nimble::Vector2f m_origin;
      Nimble::Vector2f m_axis0;
      Nimble::Vector2f m_axis1;
      float m_extent0;
      float m_extent1;
  };

}

#endif
