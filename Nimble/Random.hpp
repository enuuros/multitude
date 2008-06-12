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

#ifndef NIMBLE_RANDOM_HPP
#define NIMBLE_RANDOM_HPP

namespace Nimble {

  /// Random number generator with uniform distribution
  /** This class generates random numbers with uniform
      distribution. It uses a fast and cheap modulo-based random
      number generation.

      The random number sequence is identical on all platforms, given
      the same seed value.
  */
  class RandomUniform
  {
  public:
    RandomUniform(unsigned val = 0) : m_val(val) {}
    ~RandomUniform() {}

    /// Random numbers between 0 and 1
    inline float rand01() 
    { 
      unsigned tmp = m_val * m_randMul + 1;
      m_val = tmp;
      return (float) tmp * (1.0 / (float) ((unsigned) 0xffffffff));
    }

    /// Random numbers between 0 and x
    inline float rand0X(float x) 
    { 
      unsigned tmp = m_val * m_randMul + 1;
      m_val = tmp;
      return (float) tmp * (x / (float) ((unsigned) 0xffffffff));
    }
      
    /// Random numbers between -1 and 1
    inline float rand11() 
    { 
      unsigned tmp = m_val * m_randMul + 1;
      m_val = tmp;
      return (float) tmp * (2.0f / (float) ((unsigned) 0xffffffff)) - 1.0f;
    }

    /// Random numbers between -x and x
    inline float randXX(float x) 
    { 
      unsigned tmp = m_val * m_randMul + 1;
      m_val = tmp;
      return (float) tmp * (2.0f * x / (float) ((unsigned) 0xffffffff)) - x;
    }
    
    inline unsigned rand() 
    { 
      m_val = m_val * m_randMul + 1;
      return m_val;
    }

    // Random 2d unit vector
    inline Nimble::Vector2f randVec2() 
    {
      return Nimble::Vector2f(rand11(), rand11()).normalize();
    }

  protected:
    unsigned m_val;
    static const unsigned m_randMul = 134695621;  
  };

}

#endif
