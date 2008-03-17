#ifndef RADIANT_COLOR_HPP
#define RADIANT_COLOR_HPP

#include <Nimble/Vector4.hpp>

namespace Radiant
{

  class Color : public Nimble::Vector4f
  {
    public:
      Color();
      Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
      Color(float r, float g, float b, float a = 1.f);
      ~Color();

      void setRGBA(float r, float g, float b, float a);
      void setRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

  };

}

#endif
