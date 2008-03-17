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

#ifndef NIMBLE_MATH_HPP
#define NIMBLE_MATH_HPP

#include <math.h>

namespace Nimble {

  /// Mathematics functions and constants for Nimble.
  /** This namespace contains mathematics constants (pi, ln2 etc.) and
      basic functions. */
  namespace Math
  {
    // Constants:

    const double PI         = 3.1415926535897931;
    const double TWO_PI     = 6.2831853071795862;
    const double HALF_PI    = 1.57079632679489660;
    const double QUARTER_PI = 0.78539816339744828;

    const double SQRT2      = 1.41421356237309514547;
    const double SQRT2_PER2 = 0.70710678118654757273;
    const double EPSILON    = 1.0e-10;

    // float & double inlines:

    inline float Cos(float v)  { return cosf(v); }
    inline float Sin(float v)  { return sinf(v); }  
    inline float Tan(float v)  { return tanf(v); }  
    inline float Sqrt(float v) { return sqrtf(v); }
    inline float InvSqrt(float v) { return 1.0f / sqrtf(v); }
    inline float Exp(float v)  { return expf(v); }
    inline float Log(float v)  { return logf(v); }
    inline float Pow(float x, float y)    { return powf(x, y); }

    inline float ACos(float v)  { return acosf(v); }
    inline float ASin(float v)  { return asinf(v); }
    inline float ATan(float v)  { return atanf(v); }
    inline float ATan2(float x, float y)  { return atan2f(x, y); }
  
    inline double Cos(double v)  { return cos(v); }
    inline double Sin(double v)  { return sin(v); } 
    inline double Tan(double v)  { return tan(v); }  
    inline double Sqrt(double v) { return sqrt(v); }
    inline double InvSqrt(double v) { return 1.0 / sqrt(v); }
    inline double Exp(double v)  { return exp(v); }
    inline double Log(double v)  { return log(v); }
    inline double Pow(double x, double y)    { return pow(x, y); }

    inline double ACos(double v)  { return acos(v); }
    inline double ASin(double v)  { return asin(v); }
    inline double ATan(double v)  { return atan(v); }
    inline double ATan2(double x, double y)  { return atan2(x, y); }

    // Min & Max inlines:
  
    template <class T>
    inline T Max(T x, T y) { return x > y ? x : y; }
    template <class T>
    inline T Min(T x, T y) { return x < y ? x : y; }
    template <class T>
    inline T Abs(T x) { return x > 0 ? x : -x; }

    template <class T>
    inline T Min(T a, T b, T c) { return Min(a, Min(b, c)); }

    template <class T>
    inline T Max(T a, T b, T c) { return Max(a, Max(b,c)); }

    /// Seeks the maximum value in an vector
    /** VMax = vector maximum. */
    template <class T>
    inline T VMax(const T * vals, int n)
    {
      T v = *vals;
      const T * sentinel = vals;
      vals++;

      for(; vals < sentinel; vals++)
	if(v < *vals) v = * vals;

      return v;
    }

    /// Seeks the minimum value in an vector
    /** VMin = vector minimum. */
    template <class T>
    inline T VMin(const T * vals, int n)
    {
      T v = *vals;
      const T * sentinel = vals;
      vals++;

      for(; vals < sentinel; vals++)
	if(v > *vals) v = * vals;

      return v;
    }

    /// Checks if two (floating point) numbers are close to each other
    /** This function is usually used to check if two floating-point
	numbers are close to each other.

	@param a The first value to compare. 
	@param b The second value to compare. 
	@paramt limit The maximum difference between the values.
     */
    template <class T>
    bool IsClose(const T & a, const T & b, const T & limit)
    {
      return Abs(a - b) < limit;
    }

    inline int Round(float x) { return x > 0.0f ? (int) (x + 0.5f) : (int) (x - 0.5f); }
    inline int Round(double x) { return x > 0.0 ? (int) (x + 0.5) : (int) (x - 0.5); }

    /** Clamps (limits) x between low and high. */
    
    /* template <class T>
    inline T Max(T v1, T v2, T v3)
    {
      if(v1 > v2)
	return v1 > v3 ? v1 : v3;
      else
	return v2 > v3 ? v2 : v3;
	}*/

    /// Clamp a value between minimum and maximum values
    /** @param x The input value to limit. 
	@param low The minimum value for comparison
	@param high The maximum value for comparison
     */
    template <class T>
    inline T Clamp(T x, T low, T high) 
    { 
      if(x < low) return low;
      if(x > high) return high;
      return x;
    }

    /** Calculates the determinant of a 2x2 matrix, which is given in
	the argument values. */
    template <class T>
    inline T Det(T a, T b, T c, T d) 
    { 
      return a * d - b * c;
    }

    

  }

}

#endif
