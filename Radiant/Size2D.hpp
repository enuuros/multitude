#ifndef SIZE2D_HPP
#define SIZE2D_HPP

#include <Nimble/Vector2.hpp>

/// @todo combine Size2D+Nimble::BoundingSquare -> rect

namespace Radiant
{

  /// Given a size and target size, return a new size optionally preserving the
  /// original aspect ratio
  Nimble::Vector2i resize(Nimble::Vector2i size, Nimble::Vector2i newSize, bool keepAspect);

  /// Given aspect ratio and size to fit, return the maximum size with the
  /// aspect ratio that still fits within the constraint
  /// @param aspect aspect ratio that must be maintained
  /// @param constraint size to fit to
  Nimble::Vector2i fitToSize(float aspect, Nimble::Vector2i constaint);

}

#endif
