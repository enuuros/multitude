#ifndef DYSLEXIC_FREETYPE_HPP
#define DYSLEXIC_FREETYPE_HPP

//#include <ft2build.h>
//#include FT_FREETYPE_H

/**
 * Dyslexic is an OpenGL font rendering library based on FTGL 2.1.2.
 *
 * @author Esa Nuuros
 */
struct FT_LibraryRec_;

namespace Dyslexic
{

  bool initialize();
  
  FT_LibraryRec_ ** freetype();

  int error();
}

#endif
