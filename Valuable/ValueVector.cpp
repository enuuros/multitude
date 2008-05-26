#include "ValueVector.hpp"
#include "ValueVectorImpl.hpp"

namespace Valuable
{

  template class ValueVector<Nimble::Vector2i, int, 2>;
  template class ValueVector<Nimble::Vector3i, int, 3>;
  template class ValueVector<Nimble::Vector4i, int, 4>;

  template class ValueVector<Nimble::Vector2f, float, 2>;
  template class ValueVector<Nimble::Vector3f, float, 3>;
  template class ValueVector<Nimble::Vector4f, float, 4>;

}
