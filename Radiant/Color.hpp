
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
#ifndef RADIANT_COLOR_HPP
#define RADIANT_COLOR_HPP

#include <Nimble/Vector4.hpp>

#include <Radiant/Export.hpp>

namespace Radiant
{

  /// Utility class for color
  class MTEXPORT Color : public Nimble::Vector4f
  {
    public:
      Color();
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
