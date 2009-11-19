
#ifndef NIMBLE_MATRIX3T_IMPL_HPP
#define NIMBLE_MATRIX3T_IMPL_HPP

#include <Nimble/Matrix3.hpp>

namespace Nimble {

  template <class T>
  const Matrix3T<T> Matrix3T<T>::IDENTITY(1, 0, 0, 
					  0, 1, 0,
					  0, 0, 1);

}

#endif

