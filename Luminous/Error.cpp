/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/Error.hpp>
#include <GL/glew.h>

namespace Luminous
{

  std::string glErrorToString()
  {
    GLenum err = glGetError();

    switch(err)
    {
      case GL_NO_ERROR:
        return "GL_NO_ERROR";
        break;
      case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:  
        return "GL_STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
        break;
      case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";
        break;
      default:
        return "Unknown error occured.";
        break;
    };    
  }
}
