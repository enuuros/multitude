#include "Plane.hpp"

static const float EPSILON = 1e-5f;

namespace Nimble
{

  Plane::Plane()
  {}

  Plane::Plane(const Nimble::Vector3f & normal, float constant)
  : m_normal(normal),
    m_constant(constant)
  {}

  Plane::Plane(const Nimble::Vector3f & normal, const Nimble::Vector3f & point)
  : m_normal(normal)
  {
    m_constant = ::dot(normal, point);
  }

  float Plane::distanceTo(const Nimble::Vector3f & point) const
  {
    return ::dot(m_normal, point) - m_constant;
  }

  bool Plane::intersect(const Nimble::Vector3f & rayO, const Nimble::Vector3f & rayD, float & rayT) const
  {
    float dotDN = ::dot(rayD, m_normal);
    float dist = distanceTo(rayO);

    if(fabs(dotDN) > EPSILON) {
      // Not parallel
      rayT = -dist / dotDN;

      return true;
    }

    if(fabs(dist) <= EPSILON) {
      // Parallel
      rayT = 0.f;
      return true;
    }
    
    return false;
  }

}

