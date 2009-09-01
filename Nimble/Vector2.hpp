/* COPYRIGHT
 *
 * This file is part of Nimble.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Nimble.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef NIMBLE_VECTOR2T_HPP
#define NIMBLE_VECTOR2T_HPP

#include <Nimble/Export.hpp>
#include <Nimble/Math.hpp>

#include <iostream>

namespace Nimble {

  /** Two-dimensional vector class for 2D mathematics.
      
      Like all classed in Nimble Vector2T has been optimized for
      speed. In general, there are no safety checks in any
      functions. */

  template <class T>
  class NIMBLE_API Vector2T
  {
  public:
    /// X-component of the vector
    T		x;
    /// Y-component of the vector
    T		y;
  
    /** Default constructor, does \b not initialize the values. */
    Vector2T () {}
    Vector2T (T cx, T cy) { x = (T)cx;	y = (T)cy; }
    template <class S> Vector2T(const S * v) { x = v[0]; y = v[1]; }
    template <class S> Vector2T	(const Vector2T<S>& v) { x = (T)v.x; y = (T)v.y; }

    template <class S> Vector2T& operator=  (const Vector2T<S>& v)	{ x = (T)v.x; y = (T)v.y; return *this; }

    Vector2T&	clear		(void)					{ x = (T)(0); y = (T)(0); return *this; }
    Vector2T&	make		(T cx, T cy)		{ x = cx; y = cy; return *this; }
    Vector2T&	make		(T xy)					{ x = (xy); y = (xy); return *this; }
    T *         data() { return &x; }
    const T *   data() const { return &x; }
    bool	operator==  (const Vector2T& src) const		                { return (x == src.x && y == src.y); }
    bool	operator!=  (const Vector2T& src) const		                { return !(x == src.x && y == src.y); }
    Vector2T&	operator+=	(const Vector2T& v)				{ x += v.x, y += v.y; return *this; }		
    Vector2T&	operator-=	(const Vector2T& v)				{ x -= v.x, y -= v.y; return *this; }
    Vector2T&	operator*=	(T s)					        { x = (x*s), y = (T)(y*s); return *this; }
    Vector2T&	operator/=	(T s)					        { s = T(1)/s; x = (x*s), y = (y*s); return *this; }
    bool	isOne		(void) const					        { return (x == (T) 1 && y == (T) 1); }
    bool	isZero		(void) const					      { return (x == (T) 0 && y == (T) 0); }
    T    	length		(void) const				        { return (T)Math::Sqrt(x*x+y*y); }
    T      	lengthSqr	(void) const				      { return x*x+y*y; }
    Vector2T&	negate		(void)						      { x=-x; y=-y; return *this; }
    Vector2T&	normalize	(T len = T(1))			{ T l = length(); if (l!= T(0)) *this *= (len/l); return *this; }
    Vector2T&	limitLength	(T len)	 { T l = length(); if (l > len) *this *= (len/l); return *this; }
    Vector2T&	scale		(const Vector2T& v)				{ x *= v.x; y *= v.y; return *this; }
    Vector2T&	scale		(const T & xs, const T & ys)				{ x *= xs; y *= ys; return *this; }
    Vector2T&	descale		(const Vector2T& v)			{ x /= v.x; y /= v.y; return *this; }
    Vector2T&	rotate		(double s, double c)		{ T t = x; x = (T)(x*c+y*-s); y = (T)(t*s+y*c); return *this; }
    Vector2T&	rotate		(double angle)					{ return rotate(sin(angle), cos(angle)); }
    double	angle		(void) const				        { return atan2(double(y), double(x)); }
    Vector2T&	clampUnit	(void)						{ if(x <= (T)0.0) x = (T)0.0; else if(x >= (T)1.0) x = (T)1.0; if(y <= (T)0.0) y = (T)0.0; else if(y >= (T)1.0) y = (T)1.0; return *this; }
    Vector2T&	clamp (T low, T high)       { x = Math::Clamp(x, low, high); y = Math::Clamp(y, low, high); return * this; }

    T           minimum         (void) const { return x < y ? x : y; }
    T           maximum         (void) const { return x > y ? x : y; }
    T           sum             (void) const { return x + y; }
    /// Returns a vector with components reordered.
    Vector2T    shuffle         (int i1 = 1, int i2 = 0) const { return Vector2T(get(i1), get(i2)); }
    Vector2T    perpendicular   () const { return Vector2T(-y, x); }

    T&            get(int i)        { return ((T*)this)[i]; }
    const T&      get(int i) const  { return ((T*)this)[i]; }

    const	T&			operator[]	(int i) const		{ return ((T*)this)[i]; }
    T&			        operator[]	(int i)				{ return ((T*)this)[i]; }
  
    template <class S>
    void copy(const S * data) { x = data[0]; y = data[1]; }
  };

  template <class T> inline	Vector2T<T>	operator+	(const Vector2T<T>& v1, const Vector2T<T>& v2) { return Vector2T<T>(v1.x+v2.x, v1.y+v2.y); }
  template <class T> inline	Vector2T<T>	operator+	(const Vector2T<T>& v1, T v2) { return Vector2T<T>(v1.x+v2, v1.y+v2); }
  template <class T> inline	Vector2T<T>	operator-	(const Vector2T<T>& v1, const Vector2T<T>& v2) { return Vector2T<T>(v1.x-v2.x, v1.y-v2.y); }
  template <class T> inline	Vector2T<T>	operator-	(const Vector2T<T>& v1, T v2) { return Vector2T<T>(v1.x-v2, v1.y-v2); }
  template <class T> inline	Vector2T<T>	operator*	(const Vector2T<T>& v, const T s) { return Vector2T<T>((T)(v.x*s), (T)(v.y*s)); }
  template <class T> inline	Vector2T<T>	operator*	(const T s, const Vector2T<T>& v) { return v*s; }
  template <class T> inline	Vector2T<T>	operator/	(const Vector2T<T>& v, const double s) { T r = T(1.0/s); return v*r; }
  template <class T> inline Vector2T<T> operator/ (const Vector2T<T>& v, const T s) { return Vector2T<T>(v.x / s, v.y / s); }
  template <class T> inline	Vector2T<T>	operator-	(const Vector2T<T>& v) { return Vector2T<T>(-v.x, -v.y); }

  template <>
  inline Vector2T<int> & Vector2T<int>::operator /= (int s)
  { 
    x = x/s;
    y = y/s;
    return *this; 
  }

  template <class T>
  inline float abs(Vector2T<T> t)
  {
    return t.length();
  }

  template <class T>
  inline float dot(const Vector2T<T> &t1, const Vector2T<T> &t2)
  {
    return t1.x * t2.x + t1.y * t2.y;
  }
  
  /* Note that these overload are NOT redundant, integer math is
     different from floating point math. */
  inline Vector2T<short> operator / (const Vector2T<short>& v, const short s)
  {
    return Vector2T<short>(v.x / s, v.y / s);
  }

  inline Vector2T<int> operator / (const Vector2T<int>& v, const int s)
  {
    return Vector2T<int>(v.x / s, v.y / s);
  }

  inline Vector2T<long> operator / (const Vector2T<long>& v, const long s)
  {
    return Vector2T<long>(v.x / s, v.y / s);
  }
  
  inline Vector2T<short> operator * (const Vector2T<short>& v, const short s)
  {
    return Vector2T<short>(v.x * s, v.y * s);
  }

  inline Vector2T<int> operator * (const Vector2T<int>& v, const int s)
  {
    return Vector2T<int>(v.x * s, v.y * s);
  }

  inline Vector2T<long> operator * (const Vector2T<long>& v, const long s)
  {
    return Vector2T<long>(v.x * s, v.y * s);
  }


  template <class T>
  inline std::ostream &operator<<(std::ostream &os, const Nimble::Vector2T<T> &t)
  {
    os << t.x << ' ' << t.y;
    return os;
  }

  template <class T>
  inline std::istream &operator>>(std::istream &is, Nimble::Vector2T<T> &t)
  {
    is >> t.x;
    is >> t.y;
    return is;
  }

  namespace Math {

    template <class T>
    inline Vector2T<int> Round(const Vector2T<T>  & that)
    {
      return Vector2T<int>(Math::Round(that.x), Math::Round(that.y));
    }
  }

  typedef Vector2T<float> Vector2;
  typedef Vector2T<float> Vector2f;
  typedef Vector2T<unsigned char> Vector2ub;
  typedef Vector2T<int> Vector2i;
  typedef Vector2T<double> Vector2d;

#ifdef WIN32
#ifdef NIMBLE_EXPORT
  // In WIN32 template classes must be instantiated to be exported
  template class Vector2T<float>;
  template class Vector2T<unsigned char>;
  template class Vector2T<int>;
  template class Vector2T<double>;
#endif
#endif

  /// Line slope types.
  enum LineSlopeType
  {
    LS_VERTICAL,
    LS_SLOPING,
    LS_HORIZONTAL
  };

  /// Compute slope of line.
  /// @param lineStart, lineEnd, the line.
  /// @param slopeType, reference to int to receive slope type.
  /// @param delta, reference to Vector2f to receive delta.
  /// @return Slope value.
  inline float lineSlope(const Vector2f lineStart, const Vector2f lineEnd,
    int & slopeType, Vector2f & delta)
  {
    float   m = 0.0f;

    delta = lineEnd - lineStart;

    if(delta.x == 0.0f)
    {
      slopeType = LS_VERTICAL;
    }
    else if(delta.y == 0.0f)
    {
      slopeType = LS_HORIZONTAL;
    }
    else
    {
      slopeType = LS_SLOPING;
      m = delta.y / delta.x;
    }

    return m;
  }

  /// Test for intersection of line segments.
  /// @param line1Start, line1End, first line.
  /// @param line2Start, line2End, second line.
  /// @param interPoint, optional pointer to vector to receive the intersection point.
  /// @return true if line segments intersect.
  inline bool linesIntersect(const Vector2f line1Start, const Vector2f line1End,
    const Vector2f line2Start, const Vector2f line2End, Vector2f * const interPoint = 0)
  {
    // Check if either line has zero length

    if((line1Start == line1End) || (line2Start == line2End))
    {
      return false;
    }

    // Get slope and deltas of first line

    int   slopeType1 = 0;
    Vector2f  delta1;
    const float   m1 = lineSlope(line1Start, line1End, slopeType1, delta1);

    // Get slope and deltas of second line

    int   slopeType2 = 0;
    Vector2f  delta2;
    const float   m2 = lineSlope(line2Start, line2End, slopeType2, delta2);

    // Determine whether infinite lines cross: if so compute line parameters

    bool  cross = false;
    float   t1 = 0.0f;
    float   t2 = 0.0f;

    switch(slopeType1)
    {
      case LS_VERTICAL:
      switch(slopeType2)
      {
        case LS_VERTICAL:
        // Lines parallel - no intersection point
        break;

        case LS_SLOPING:
        {
          cross = true;
          t2 = (line1Start.x - line2Start.x) / delta2.x;
          t1 = (line2Start.y + (t2 * delta2.y) - line1Start.y) / delta1.y;
        }
        break;

        case LS_HORIZONTAL:
        {
          cross = true;
          t1 = (line2Start.y - line1Start.y) / delta1.y;
          t2 = (line1Start.x - line2Start.x) / delta2.x;
        }
        break;
      }
      break;

      case LS_SLOPING:
      switch(slopeType2)
      {
        case LS_VERTICAL:
        {
          cross = true;
          t1 = (line2Start.x - line1Start.x) / delta1.x;
          t2 = (line1Start.y + (t1 * delta1.y) - line2Start.y) / delta2.y;
        }
        break;

        case LS_SLOPING:
        {
          if(m1 == m2)
          // Lines parallel - no intersection point
          {
          }
          else
          {
            cross = true;
            const float   value = delta2.x * delta1.y;
            const float   divisor = 1.0f - (delta1.x * delta2.y) / value;
            t1 = (line2Start.y / delta1.y + (line1Start.x * delta2.y) / value
              - (line2Start.x * delta2.y) / value - line1Start.y / delta1.y) / divisor;
            t2 = (line1Start.x + t1 * delta1.x - line2Start.x) / delta2.x;
          }
        }
        break;

        case LS_HORIZONTAL:
        {
          cross = true;
          t1 = (line2Start.y - line1Start.y) / delta1.y;
          t2 = (line1Start.x + (t1 * delta1.x) - line2Start.x) / delta2.x;
        }
        break;
      };
      break;

      case LS_HORIZONTAL:
      switch(slopeType2)
      {
        case LS_VERTICAL:
        {
          cross = true;
          t1 = (line2Start.x - line1Start.x) / delta1.x;
          t2 = (line1Start.y - line2Start.y) / delta2.y;
        }
        break;

        case LS_SLOPING:
        {
          cross = true;
          t2 = (line1Start.y - line2Start.y) / delta2.y;
          t1 = (line2Start.x + t2 * delta2.x - line1Start.x) / delta1.x;
        }
        break;

        case LS_HORIZONTAL:
        // Lines parallel - no intersection point
        break;
      }
      break;
    }

    if(!cross)
    {
      return false;
    }

    // Compute point of intersection

    if(interPoint)
    {
      * interPoint = Vector2f(line1Start.x + t1 * delta1.x, line1Start.y + t1 * delta1.y);
    }

    // Return true only if point of intersection is on both lines

    return(t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f);
  }


} // namespace


template <class S, class T>
inline S &operator<<(S &os, const Nimble::Vector2T<T> &t)
{
  os << t.x << ' ' << t.y;
  return os;
}

#endif
