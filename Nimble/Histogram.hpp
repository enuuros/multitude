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

#ifndef NIMBLE_HISTOGRAM_HPP
#define NIMBLE_HISTOGRAM_HPP

#include <Nimble/Export.hpp>

#ifdef WIN32
#include <Win32/WinPort/include/WinPort.h>
#endif

#include <string>

namespace Nimble {

  /// Histogram calculation
  /** This class can be used to calculate histograms of all kinds of
    values. 

    @author Tommi Ilmonen */
  template <class T, int N> 
    class NIMBLE_API Histogram
    {
      public:
        Histogram() : m_count(0) {}
        ~Histogram() {}

        /// Sets all histogram bins to zero
        void clear() { bzero(m_data, sizeof(m_data)); m_count = 0; }

        /// Adds the histogram bin "index" by one
        void put(int index) { m_data[index]++; m_count++; }

        /// Adds the histogram bin "index" by one
        /** It is ok to exceed the array towards top - i.e. index exceeds
          the histogram bin count. */
        void putSafe(int index) 
        {
          if(index < N)
            m_data[index]++;
          else
            m_data[N - 1]++;
          m_count++; 
        }

        /** It is ok to exceed the array towards any direction.. */
        void putSafest(int index) 
        {
          if(index >= N)
            m_data[N - 1]++;
          else if(index < 0)
            m_data[0]++;
          else
            m_data[index]++;

          m_count++; 
        }

        /// Find the bin below which there are required number of samples
        /** @arg ratio relative ratio for selection (0-1). */
        int getLowValueRelative(float ratio) const
        { return getLowValueFixed((int) (ratio * m_count)); }

        /// Find the bin above which there are required number of samples
        /** @arg ratio relative ratio for selection (0-1). */
        int getHighValueRelative(float ratio) const
        { return getHighValueFixed((int) (ratio * m_count)); }

        /// Find the bin below which there are required number of samples
        /** @arg need required sum of bins (0-count()). */
        int getLowValueFixed(int need) const
        {
          int count = 0;
          for(int i = 0; i < N ; i++) {
            count += m_data[i];
            if(count >= need)
              return i;
          }
          return N - 1;
        }

        /// Find the bin above which there are required number of samples
        /** @arg need required sum of bins (0-count()). */
        int getHighValueFixed(int need) const
        {
          int count = 0;
          for(int i = N - 1; i >= 0 ; i--) {
            count += m_data[i];
            if(count >= need)
              return i;
          }
          return 0;
        }

        /// Add the values from another histogram to this histogram
        void add(const Histogram & that)
        {
          for(int i = 0; i < N; i++)
            m_data[i] += that.m_data[i];
          m_count += that.m_count;
        }

        /// Returns the sum of all bin counts
        int count() const { return m_count; }

      protected:

        T   m_data[N];
        int m_count;
    };

  typedef Histogram<unsigned int,256> Histogramu256;
  typedef Histogram<int,256> Histogrami256;

#ifdef WIN32
#ifdef NIMBLE_EXPORT
  // In WIN32 template classes must be instantiated to be exported
  template class Histogram<unsigned int, 256>;
  template class Histogram<int, 256>;
#endif
#endif

} // namespace

#endif
