
#ifndef NIMBLE_MATRIX4T_IMPL_HPP
#define NIMBLE_MATRIX4T_IMPL_HPP

#include <Nimble/Matrix4.hpp>

namespace Nimble {

  template <class T>
  const Matrix4T<T> Matrix4T<T>::IDENTITY(1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0,
					  0, 0, 0, 1);

}

#endif

