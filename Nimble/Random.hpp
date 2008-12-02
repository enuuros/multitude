/* COPYRIGHT
 *
 * This file is part of ConfigReader.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
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

#include <Nimble/Vector2.hpp>

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
      return (float) tmp * (1.0f / (float) ((unsigned) 0xffffffff));
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

    /// Random numbers between min and max
    inline float randMinMax(float min, float max) 
    { 
      return rand0X(max - min) + min;
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

    static RandomUniform & instance() { return m_instance; }

  private:
    unsigned m_val;
    static const unsigned m_randMul = 134695621;  
    static RandomUniform  m_instance;
  };

  /// RandomGaussian generates pseudo-random numbers from a normal (gaussian) distribution.
  class RandomGaussian
  {
    public:
      /// Construct a generator with given parameters for the distribution of
      /// the random numbers.
      /// @param mean the mean of the normal distribution
      /// @param stdDev the standard deviation for the normal distribution
      /// @param seed seed value for the pseudo-random sequence
      RandomGaussian(float mean = 0.0f, float stdDev = 1.0f, unsigned seed = 0) : m_uniform(seed), m_mean(mean), m_stdDev(stdDev) {}

      /// Generate a random number from the distribution
      /// @return a pseudo-random number
      inline float rand() {
        float x1, x2, rsq;

        do {
          // Pick two uniform numbers within a unit-square and test if they are
          // within a unit-circle, if not, try again
          x1 = 2.0f * m_uniform.rand01() - 1.0f;
          x2 = 2.0f * m_uniform.rand01() - 1.0f;
          rsq = x1 * x1 + x2 * x2;
        } while(rsq >= 1.0f || rsq == 0.0f);

        // Box-Muller transformation and return the other number
        float fac = sqrt((-2.0f * log(rsq)) / rsq);
        return (x2 * fac) * m_stdDev + m_mean;
      }

    private:
      RandomUniform m_uniform;
      float m_mean;
      float m_stdDev;
  };
}

#endif
