#include "GPUFont.hpp"

namespace Dyslexic
{

  void GPUFont::render(const char * str, const Nimble::Matrix3 & m)
  {
    internalRender(str, strlen(str), m);
  }

  void GPUFont::render(const wchar_t * str, const Nimble::Matrix3 & m)
  {
    internalRender(str, wcslen(str), m);
  }

  void GPUFont::render(const std::string & str, const Nimble::Matrix3 & transform)
  {
    internalRender(str.c_str(), str.size(), transform);
  }

  void GPUFont::render(const std::string & str, const Nimble::Vector2 & location)
  {
    internalRender(str.c_str(), str.size(),
		   Nimble::Matrix3::translate2D(location));
  }

  void GPUFont::render(const std::wstring & str, const Nimble::Vector2 & location)
  {
    internalRender(str.c_str(), str.size(),
		   Nimble::Matrix3::translate2D(location));
  }

  void GPUFont::render(const std::string & str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str.c_str(), str.size(), transform);
  }

  void GPUFont::render(const char * str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str, strlen(str), transform);
  }

  void GPUFont::render(const wchar_t * str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str, wcslen(str), transform);
  }

  void GPUFont::render(const std::wstring & str)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    internalRender(str.c_str(), str.length(), transform);
  }

  void GPUFont::render(const char * str, Nimble::Vector2 loc)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    transform[0][2] = loc.x;
    transform[1][2] = loc.y;
    internalRender(str, strlen(str), transform);    
  }

  void GPUFont::render(const char * str, float scale, Nimble::Vector2 loc)
  {
    Nimble::Matrix3 transform;
    transform.identity();
    transform[0][0] = scale;
    transform[1][1] = scale;
    transform[0][2] = loc.x;
    transform[1][2] = loc.y;
    internalRender(str, strlen(str), transform);    
  }

  void GPUFont::render(const std::wstring & str, const Nimble::Matrix3 & transform)
  {
    internalRender(str.c_str(), str.length(), transform);
  }

}
