/* -*- C++ -*- */
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

#ifndef NIMBLE_RECT_HPP
#define NIMBLE_RECT_HPP

#include <Nimble/Vector2.hpp>
#include <Nimble/Matrix3.hpp>

namespace Nimble {

  /// An axis-aligned rectangle.
  template <class T>
  class RectT
  {
  public:
    RectT()
    : m_low(0, 0),
      m_high(-1, -1) 
    {}

    inline bool isEmpty() const { return m_low.x > m_high.x || m_low.y > m_high.y; }

    RectT(const Vector2T<T> & lowHigh) 
      : m_low(lowHigh), m_high(lowHigh) {}
    RectT(const Vector2T<T> * lowHigh) 
      : m_low(*lowHigh), m_high(*lowHigh) {}
    RectT(const Vector2T<T> & low, const Vector2T<T> & high) 
      : m_low(low), m_high(high) {}
    RectT(T xlow, T ylow, T xhigh, T yhigh) 
      : m_low(xlow, ylow), m_high(xhigh, yhigh) {}
    ~RectT() {}

    RectT & operator=(const RectT & bs) { m_low = bs.m_low; m_high = bs.m_high; return *this; }

    /// Multiply the coordinates b v
    void scale(T v) { m_low = m_low * v; m_high = m_high * v; }
    inline void scale(const Vector2T<T> &v);
    
    /// Add v to the coordinates
    void move(const Vector2T<T> &v) { m_low += v; m_high += v; }
    void moveHighClamped(const Vector2T<T> &v) 
    { 
      m_high += v; 
      for(int i = 0 ; i < 2; i++)
	if(m_high[i] < m_low[i])
	  m_high[i] = m_low[i]; 
    }

    void clear() { m_low.clear(); m_high.clear(); }
    void clear(const Vector2T<T> &v) { m_low = m_high = v; }

    inline void expand(const Vector2T<T> &v);
    inline void expand(const Vector2T<T> &v, T radius);
    inline void expand(const RectT &b);

    Vector2T<T> & low() { return m_low; }
    const Vector2T<T> & low() const { return m_low; }
    Vector2T<T> & high() { return m_high; }
    const Vector2T<T> & high() const { return m_high; }

    Vector2T<T> lowHigh() const { return Vector2T<T>(m_low.x, m_high.y); }
    Vector2T<T> highLow() const { return Vector2T<T>(m_high.x, m_low.y); }

    void set(T lx, T ly, T hx, T hy)
    { m_low.make(lx, ly); m_high.make(hx, hy); }

    void set(const Vector2T<T> &low, const Vector2T<T> &high)
    { m_low = low; m_high = high; }

    void set(const Vector2T<T> &point)
    { m_low = m_high = point; }

    void set(const Vector2T<T> &point, T radius)
    { m_low = point - radius; m_high = point + radius; }

    void setLow(const Vector2T<T> &low)
    { m_low = low; }

    void setHigh(const Vector2T<T> &high)
    { m_high = high; }

    void setLowX(const T lowX)
    { m_low.x = lowX; }

    void setLowY(const T lowY)
    { m_low.y = lowY; }

    void setHighX(const T highX)
    { m_high.x = highX; }

    void setHighY(const T highY)
    { m_high.y = highY; }

    inline Vector2T<T> center() const { return (m_low + m_high) * (T) 0.5; }
    inline Vector2T<T> span() const { return m_high - m_low; }
    inline Vector2T<T> topCenter() const;

    inline T width()  const { return m_high.x - m_low.x; }
    inline T height() const { return m_high.y - m_low.y; }
    inline Vector2T<T> size() const { return m_high - m_low; }

    inline bool intersects(const RectT &) const;
    inline bool contains(T x, T y) const;
    inline bool contains(Vector2T<T> ) const;
    inline bool contains(const RectT &b) const;
    inline T    distance(const RectT &b) const;

    inline Vector2T<T> clamp(const Vector2T<T> &) const;

    inline void transform(const Matrix3T<T>& m);
    inline RectT quarter(int row, int col) const;
  
  protected:
    Vector2T<T> m_low, m_high;
  };

  template <class T> 
  inline void RectT<T>::expand(const Vector2T<T> &v)
  {
    if(v[0] < m_low[0]) m_low[0] = v[0];
    if(v[1] < m_low[1]) m_low[1] = v[1];

    if(v[0] > m_high[0]) m_high[0] = v[0];
    if(v[1] > m_high[1]) m_high[1] = v[1];
  }

  template <class T> 
  inline void RectT<T>::expand(const Vector2T<T> &v, T radius)
  {
    expand(v - Vector2T<T>(radius, radius));
    expand(v + Vector2T<T>(radius, radius));
  }

  template <class T> 
  inline void RectT<T>::expand(const RectT &b)
  {
    if(isEmpty()) {
      *this = b;
      return;
    }

    if(b.isEmpty()) 
      return;

    if(b.m_low[0] < m_low[0]) m_low[0] = b.m_low[0];
    if(b.m_low[1] < m_low[1]) m_low[1] = b.m_low[1];

    if(b.m_high[0] > m_high[0]) m_high[0] = b.m_high[0];
    if(b.m_high[1] > m_high[1]) m_high[1] = b.m_high[1];
  }

  template <class T> 
  void RectT<T>::scale(const Vector2T<T> &v)
  {
    m_low[0] *= v[0];
    m_low[1] *= v[1];

    m_high[0] *= v[0];
    m_high[1] *= v[1];
  }

  template <class T> 
  Vector2T<T> RectT<T>::topCenter() const
  { 
    return Vector2T<T>((m_low.x + m_high.x) * 0.5, m_high.y);
  }

  template <class T> 
  bool RectT<T>::intersects(const RectT &b) const
  {
    for(int i = 0; i < 2; i++) {
      if(b.m_high[i] < m_low[i] || b.m_low[i] > m_high[i])
	return false;
    }

    return true;
  }


  template <class T> 
  inline bool RectT<T>::contains(T x, T y) const
  {
    return ((x >= m_low[0]) && (x <= m_high[0]) &&
	    (y >= m_low[1]) && (y <= m_high[1]));  
  }

  template <class T> 
  inline bool RectT<T>::contains(Vector2T<T> v) const
  {
    return ((v[0] >= m_low[0]) && (v[0] <= m_high[0]) && (v[1] >= m_low[1]) && (v[1] <= m_high[1]));  
  }

  template <class T> 
  inline bool RectT<T>::contains(const RectT &b) const
  {
    return ((b.m_low[0] >= m_low[0]) && (b.m_high[0] <= m_high[0]) &&
	    (b.m_low[1] >= m_low[1]) && (b.m_high[1] <= m_high[1]));
  }

  template <class T> 
  inline T RectT<T>::distance(const RectT &b) const
  {
    Vector2T<T> mind;

    for(int i = 0; i < 2; i++) {

      if(b.m_high[i] < m_low[i])
        mind[i] = m_low[i] - b.m_high[i];
      else if(b.m_low[i] > m_high[i])
        mind[i] = b.m_low[i] - m_high[i];
      else
        mind[i] = 0;

    }

    return mind.maximum();
  }

  template <class T> 
  inline Vector2T<T> RectT<T>::clamp(const Vector2T<T> &v) const
  {
    int i;
    Vector2T<T> r(v);

    for(i=0; i < 2; i++)
      if(r[i] < m_low[i]) r[i] = m_low[i];

    for(i=0; i < 2; i++)
      if(r[i] > m_high[i]) r[i] = m_high[i];

    return r;
  }

  
  template <>
  inline Vector2T<int> RectT<int>::center() const
  {
    return (m_low + m_high) / 2; 
  }
 
  template<class T>
  void RectT<T>::transform(const Matrix3T<T>& m)
  {
    Vector2T<T> v0(m_low.x, m_low.y);
    Vector2T<T> v1(m_high.x, m_low.y);
    Vector2T<T> v2(m_high.x, m_high.y);
    Vector2T<T> v3(m_low.x, m_high.y);

    Vector3T<T> t0 = m * v0;
    Vector3T<T> t1 = m * v1;
    Vector3T<T> t2 = m * v2;
    Vector3T<T> t3 = m * v3;

    T xmin = Math::Min(t0.x, t1.x);
    T ymin = Math::Min(t0.y, t1.y);
    T xmax = Math::Max(t0.x, t1.x);
    T ymax = Math::Max(t0.y, t1.y);

    xmin = Math::Min(xmin, t2.x);
    ymin = Math::Min(ymin, t2.y);
    xmax = Math::Max(xmax, t2.x);
    ymax = Math::Max(ymax, t2.y);

    xmin = Math::Min(xmin, t3.x);
    ymin = Math::Min(ymin, t3.y);
    xmax = Math::Max(xmax, t3.x);
    ymax = Math::Max(ymax, t3.y);

    m_low.x = xmin;
    m_low.y = ymin;
    m_high.x = xmax;
    m_high.y = ymax;       
  }

  template<class T>
  inline RectT<T> RectT<T>::quarter(int row, int col) const
  {
    RectT<T> res;

    Vector2T<T> size = span() * 0.5f;
    if(row)
      res.m_low.y = m_low.y + size.y;
    if(col)
      res.m_low.x = m_low.x + size.x;
    res.m_high = res.m_low + size;
    return res;
  }
 
  typedef RectT<float> Rect;
  typedef RectT<float> Rectf;
  typedef RectT<int> Recti;
  typedef RectT<double> Rectd;

  /** Multiplies the location vectors with some matrix. */
  /*
    template <class M>
    inline Rect operator * (const M &m, const Rect &b)
    {
    Vector2T<T> tmp, low = b.low(), high = b.high();

    Rect square;
    square.set(m * low);
  
    for(uint x = 0; x < 2; x++) {
    for(uint y = 0; y < 2; y++) {
    tmp.x = x ? low.x : high.x;
    tmp.y = y ? low.y : high.y;

    square.expand(m * tmp);
    }
    }

    return square;
    }
  */
}

#endif

