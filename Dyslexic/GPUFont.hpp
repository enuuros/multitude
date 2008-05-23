#ifndef DYSLEXIC_GPU_FONT_HPP
#define DYSLEXIC_GPU_FONT_HPP

#include <Nimble/Matrix3.hpp>

namespace Dyslexic
{
  class CPUFont;

  /// A common interface for all fonts that reside on the GPU
  class EXPORT GPUFont
  {
    public:
      virtual ~GPUFont() {}

      virtual CPUFont * cpuFont() = 0;

      void render(const char * str, const Nimble::Matrix3 & transform);
      void render(const wchar_t * str, const Nimble::Matrix3 & transform);

      void render(const std::string & str);
      void render(const std::string & str, const Nimble::Matrix3 & transform);
      void render(const std::string & str, const Nimble::Vector2 & location);

      void render(const std::wstring & str);
      void render(const std::wstring & str, const Nimble::Matrix3 & transform);
      void render(const std::wstring & str, const Nimble::Vector2 & location);

      void render(const char * str);
      void render(const wchar_t * str);

      void render(const char * str, Nimble::Vector2 loc);
      void render(const char * str, float scale, Nimble::Vector2 loc);

    protected:
      // The actual rendering methods, override these in derived classes
      virtual void internalRender(const char * str, int n, const Nimble::Matrix3 & transform) = 0;
      virtual void internalRender(const wchar_t * str, int n, const Nimble::Matrix3 & transform) = 0;
  };

}

#endif
