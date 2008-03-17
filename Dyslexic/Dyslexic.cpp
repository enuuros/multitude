#include "Dyslexic.hpp"
#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Dyslexic
{

  static FT_Library * g_library = 0;
  static FT_Error g_error = 0;

  bool initialize()
  {
    if(g_library) 
      return true;

    g_library = new FT_Library;

    g_error = FT_Init_FreeType(g_library);
    if(g_error) {
      delete g_library;
      g_library = 0;
      return false;
    }

    return true;    
  }

  FT_Error error() 
  {
    return g_error;
  }

  FT_Library * freetype()
  {
    if(!g_library) {
      bool result = initialize();
      assert(result);
    }

    return g_library;
  }

}
