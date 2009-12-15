#ifndef NIMBLE_MATRIX4T_IMPL_HPP
#define NIMBLE_MATRIX4T_IMPL_HPP

#include <Nimble/Matrix4.hpp>

namespace Nimble {

  template <class T>
  const Matrix4T<T> Matrix4T<T>::IDENTITY(1, 0, 0, 0,
                      0, 1, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1);

  template <class T>
  Matrix4T<T> Matrix4T<T>::translate3D(const Vector3T<T> & v)
  {
    return Matrix4T(1, 0, 0, v[0],
                    0, 1, 0, v[1],
                    0, 0, 1, v[2],
                    0, 0, 0, 1);
  }

  template <class T>
  Matrix4T<T> Matrix4T<T>::scale3D(const Vector3T<T> & v)
  {
    return Matrix4T(v[0], 0, 0, 0,
                    0, v[1], 0, 0,
                    0, 0, v[2], 0,
                    0, 0, 0, 1);

  }


}

#endif

