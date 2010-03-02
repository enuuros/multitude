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

  Rectangle::Rectangle(Nimble::Vector2f size, const Nimble::Matrix3f & m)
  {
    // Transform the points
    m_origin = (m * Vector2f(0, 0)).xy();
    Vector2f c0 = (m * Vector2f(size.x, 0.f)).xy();
    Vector2f c1 = (m * Vector2f(0.f, size.y)).xy();

    // Compute the axii and extents
    m_axis0 = c0 - m_origin;
    m_axis1 = c1 - m_origin;

    m_extent0 = m_axis0.length();
    m_extent1 = m_axis1.length();

    m_axis0.normalize();
    m_axis1.normalize();
  }

  bool Rectangle::inside(Nimble::Vector2f p) const
  {
    p -= m_origin;

    float u = dot(p, m_axis0);
    float v = dot(p, m_axis1);

    return (0 <= u && u <= m_extent0) && (0 <= v && v <= m_extent1);
  }

}
