/* COPYRIGHT
 *
 * This file is part of Mustajuuri LGPL modules.
 *
 * Author: Tommi Ilmonen, 1999-2004.
 * e-mail: firstname.lastname(at)hut.fi
 *
 * http://www.tml.hut.fi/~tilmonen/mustajuuri/
 *
 * This file is licensed under the Lesser General Public License
 * (LGPL) - version 2.1. Extra clauses can be found from the file
 * MUSTAJUURI_LGPL and further explanations from the file
 * MUSTAJUURI_LICENSE. The license text can be found in file
 * "LGPL.txt".
 *
 * If you want a parallel license (for commercial reasons for example),
 * you should negotiate the matter with the author(s).
 *    
 */

#include "RingBuffer.hpp"

namespace Nimble {

  float lag3IntCoeffs[LAG3_INTCOEFFS * 4];

  /// Initializes the lag3IntCoeffs structure.
  bool lag3IntCoeffsCalculate()
  {
    for(unsigned i=0; i < LAG3_INTCOEFFS; i++) {
      double delay = (double) i / (double) LAG3_INTCOEFFS; 
    
      /*const ulong ndelay = (ulong) delay;
	const double d = delay - (double) ndelay;*/
      const double d = delay + 1.0;
      const double dm1 = d-1;
      const double dm2 = d-2;
      const double dm3 = d-3;
      const double dm12p6 = dm1 * dm2 * (1.0 / 6.0);
      const double dm03p2 = d * dm3 * 0.5;

      float * target = lag3IntCoeffs + i * 4;

      *target++ = (float) (-dm12p6 * dm3);
      *target++ = (float) ( dm03p2 * dm2);
      *target++ = (float) (-dm03p2 * dm1);
      *target   = (float) ( dm12p6 * d);
    }
    return true;
  }

  class DspFoo
  {
  public:
    DspFoo() { lag3IntCoeffsCalculate(); }
  };

  static DspFoo foo;

}
