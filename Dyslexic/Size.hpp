#ifndef DYSLEXIC_SIZE_HPP
#define DYSLEXIC_SIZE_HPP

struct FT_SizeRec_;
struct FT_FaceRec_;

namespace Dyslexic
{

  /// Size provides access to the size information in a font.
  class Size
  {
    public:
      Size();
      ~Size();

      bool charSize(FT_FaceRec_ ** face, int pointSize, int xRes, int yRes);

      int charSize() const { return m_size; }

      float ascender() const;
      float descender() const;

      float width() const;
      float height() const;
      float underline() const { return 0.f; }

      int error() const { return m_error; }

    private:
      FT_FaceRec_ ** m_ftFace;
      FT_SizeRec_ * m_ftSize;

      int m_size;
      int m_xRes;
      int m_yRes;

      int m_error;
  };

}

#endif
