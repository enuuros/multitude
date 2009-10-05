/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Radiant/Color.hpp>
#include <Radiant/Trace.hpp>

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <WinPort.h>
#endif


int main(int argc, char ** argv)
{
  const int ncolors = 5;

  Radiant::Color colors[ncolors] = {
    Radiant::Color("#AABBCC"),
    Radiant::Color("#11223344"),
    Radiant::Color(0, 100, 255, 80),
    Radiant::Color(0.1f, 0.2f, 0.3f, 0.4f),
    Radiant::Color(Nimble::Vector4f(0.1f, 0.2f, 0.3f, 0.4f))
  };

  for(int i = 0; i < ncolors; i++) {
    Radiant::Color c = colors[i];
    Radiant::info("Color %d RGBA components are: %.3f %.3f %.3f %.3f", i+1,
		  c[0], c[1], c[2], c[3]);
  }
  
  return 0;
}

