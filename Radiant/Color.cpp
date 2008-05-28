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
#include "Color.hpp"

namespace Radiant
{

  Color::Color()
  : Nimble::Vector4f(0.f, 0.f, 0.f, 1.f)
  {}

  Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
  {
    setRGBA(r, g, b, a);
  }

  Color::Color(float r, float g, float b, float a)
  {
    setRGBA(r, g, b, a);
  }

  Color::Color(const Nimble::Vector4f & v) 
  {
    setRGBA(v.x, v.y, v.z, v.w);
  }

  Color::~Color()
  {}

  void Color::setRGBA(float r, float g, float b, float a)
  {
    make(r, g, b, a);
  }

  void Color::setRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
  {
    float rf = (float)r / 255.f;
    float gf = (float)g / 255.f;
    float bf = (float)b / 255.f;
    float af = (float)a / 255.f;

    make(rf, gf, bf, af);
  }

}
