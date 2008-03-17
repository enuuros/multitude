#ifndef SIZE2D_HPP
#define SIZE2D_HPP

#include <Nimble/Vector2.hpp>

/// @todo combine Size2D+Nimble::BoundingSquare -> rect

namespace Radiant
{

  Nimble::Vector2i resize(const Nimble::Vector2i & oldSize, const Nimble::Vector2i & newSize, bool keepAspect);

}

#endif
