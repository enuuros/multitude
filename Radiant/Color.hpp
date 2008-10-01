
/* COPYRIGHT
 *
 * This file is part of Radiant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Radiant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
#ifndef RADIANT_COLOR_HPP
#define RADIANT_COLOR_HPP

#include <Nimble/Vector4.hpp>

#include <Radiant/Export.hpp>

namespace Radiant
{

  /// Utility class for color management & conversion
  /** Color implements a classical RGBA color in floating-point format. 
      
      <b>Caveat:</b> You need to careful with the floating-point and
      integer versions of the functions. For integers the valid range
      is 0-255, while for floating point values it is 0.0-1.0.
   */
  class RADIANT_API Color : public Nimble::Vector4f
  {
    public:
      Color();
      /** Decode color from a hex-string. The string must be in
	  typical hex format and start with hash. If the string
	  contains 8 number values, then the last to are interpreted
	  as alpha. By default the alpha is set to 255 (fully opaque).
	  
	  Example arguments are #000000 (black), #FFFFFF (white),
	  #FF0000 (red) and #FF000088 (transparent red).

      */
      Color(const char * color);
      Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
      Color(int r, int g, int b, int a = 255);
      Color(float r, float g, float b, float a = 1.f);
      Color(const Nimble::Vector4f & v);
      ~Color();

      void setRGBA(float r, float g, float b, float a);
      void setRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

  };

}

#endif
