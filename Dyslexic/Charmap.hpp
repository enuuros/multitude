#ifndef DYSLEXIC_CHARMAP_HPP
#define DYSLEXIC_CHARMAP_HPP

#include "Face.hpp"
#include <map>

struct FT_FaceRec_;

namespace Dyslexic
{

  class Charmap
  {
    public:
      Charmap(Face * face);
      ~Charmap();

      bool charmap(int encoding);
      int encoding() const { return m_ftEncoding; }

      unsigned int glyphListIndex(unsigned int charCode);
      unsigned int fontIndex(unsigned int charCode);
      void insertIndex(unsigned int charCode, unsigned int index);

      int error() const { return m_error; }

    private:
      int m_ftEncoding;
      FT_FaceRec_ * m_ftFace;

      typedef std::map<unsigned long, unsigned long> container;

      container m_charmap;

      int m_error;
  };

}

#endif

