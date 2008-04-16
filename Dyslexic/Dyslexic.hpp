#ifndef DYSLEXIC_FREETYPE_HPP
#define DYSLEXIC_FREETYPE_HPP

//#include <ft2build.h>
//#include FT_FREETYPE_H

struct FT_LibraryRec_;

/** Dyslexic is an OpenGL font rendering library.
 *
 * Dyslexic is a C++ OpenGL font rendering library based on FTGL
 * 2.1.2. It was built to be used in multithreaded rendering.
 *
 * @author Esa Nuuros
 */
namespace Dyslexic
{

  bool initialize();
  
  FT_LibraryRec_ ** freetype();

  int error();
}

#endif
