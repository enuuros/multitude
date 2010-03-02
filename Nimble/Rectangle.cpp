#include "Rectangle.hpp"

namespace Nimble
{
  Rectangle::Rectangle()
  {}

  Rectangle::Rectangle(Nimble::Vector2f origin, Nimble::Vector2f a0, float e0, Nimble::Vector2f a1, float e1)
      : m_origin(origin),
      m_axis0(a0),
      m_axis1(a1),
      m_extent0(e0),
      m_extent1(e1)
  {}

  bool Rectangle::inside(Nimble::Vector2f p) const
  {
    p -= m_origin;

    float u = dot(p, m_axis0);
    float v = dot(p, m_axis1);

    return (0 <= u && u <= m_extent0) && (0 <= v && v <= m_extent1);
  }

}
