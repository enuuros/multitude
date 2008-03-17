/* COPYRIGHT
 *
 * This file is part of ConfigReader.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ConfigReader.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef NIMBLE_MATRIX2T_HPP
#define NIMBLE_MATRIX2T_HPP

#include <Nimble/Math.hpp>
#include <Nimble/Vector2.hpp>
#include <assert.h>

namespace Nimble {

  /// 2x2 row-major matrix class
  /** The rows of this matrix are of type Nimble::Vector2T<T>. */
  template <class T>
  class Matrix2T
  {
  public:
    /// Create a matrix without initializing the elements
    Matrix2T() {}
    Matrix2T(T v11, T v12, T v21, T v22) { m[0].make(v11, v12); m[1].make(v21, v22); }
    Matrix2T(const Matrix2T &that) { m[0] = that.m[0]; m[1] = that.m[1]; }
    /// Empty destructor
    /** This method is defined because otherwise so compilers might
	complain. We expect that a decent compiler knows how to
	eliminate this function. */
    ~Matrix2T() {}

    /// Returns a reference to a row
    Nimble::Vector2T<T>&       row(int i)             { return m[i]; }
    /// Returns a constant reference to a row
    const Nimble::Vector2T<T>& row(int i) const       { return m[i]; }
    /// Returns a copy of a column
    Nimble::Vector2T<T>        column(int i) const    { return Vector2T<T>(m[0][i],m[1][i]); }
    /// Returns a reference to a row
    Nimble::Vector2T<T>&       operator[](int i)      { return row(i); }
    /// Returns a constant reference to a row
    const Nimble::Vector2T<T>& operator[](int i) const{ return row(i); }
    /// Sets the given matrix element
    void               set(int r, int c, T v) { m[r][c] = v; }
    /// Gets the given matrix element
    T                  get(int r, int c) const{ return m[r][c]; }
    /// Returns a pointer to the matrix data.
    T *                data() { return m[0].data(); }
    /// Returns a constant pointer to the matrix data.
    const T *           data() const { return m[0].data(); }

    /// Transpose this matrix
    void               transpose()            { swap(m[0][1],m[1][0]); }
    /// Set all matrix elements to zero
    void               clear()                { m[0].clear(); m[1].clear(); } 
    /// Create an identity matrix
    void               identity()             { m[0].make(1.0, 0.0); m[1].make(0.0, 1.0);}
    /// Create a rotation matrix
    void               rotate(T);
    /// Create a scaling matrix
    void               scale(T s)             { m[0].make(s, 0.0); m[1].make(0.0, s); }

    /// Returns the number of rows in this matrix type
    static int         rows() { return 2; }
    /// Returns the number of columns in this matrix type
    static int         columns() { return 2; }

    /// Returns a rotation matrix
    static Matrix2T rotation(T r) { T c = Math::Cos(r); T s = Math::Sin(r); return Matrix2T(c, -s, s, c); }
    /// Returns a scaling matrix
    static Matrix2T scaling(T s) { Matrix2T m; m.identity(); m.set(0, 0, s); m.set(1, 1, s); return m; }

  private:
    static void swap(T &a, T& b);
    Vector2T<T> m[2];
  };

  template <class T>
  inline void Matrix2T<T>::swap(T &a, T& b)
  {
    T t = a;
    a = b;
    b = t;
  }

  template <class T>
  inline void Matrix2T<T>::rotate(T a)
  {
    T ca = Math::Cos(a);
    T sa = Math::Sin(a);
    m[0].make(ca, -sa);
    m[1].make(sa, ca);
  }

  template <class T>
  inline Matrix2T<T> operator*(const Matrix2T<T>& m1, const Matrix2T<T>& m2)
  {
    Matrix2T<T> res;

    for(int i = 0; i < 2; i++) {
      Vector2T<T> t = m2.column(i);
      res[0][i] = dot(m1.row(0),t);
      res[1][i] = dot(m1.row(1),t);
    }

    return res;
  }

  template <class T>
  inline Vector2T<T> operator*(const Matrix2T<T>& m1,const Vector2T<T>& m2)
  {
    Vector2T<T> res;
    for(int i = 0; i < 2; i++)
      res[i] = dot(m1.row(i),m2);
    return res;
  }

  template <class T>
  inline Vector2T<T> operator*(const Vector2T<T>& m2, const Matrix2T<T>& m1)
  {
    Vector2T<T> res;
    for(int i = 0; i < 2; i++)
      res[i] = dot(m1.column(i),m2);
    return res;
  }

  typedef Matrix2T<float> Matrix2;

}

#endif

