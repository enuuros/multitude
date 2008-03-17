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

#ifndef LUMINOUS_GLSLSHADEROBJECT_HPP
#define LUMINOUS_GLSLSHADEROBJECT_HPP

#include <GL/glew.h>

namespace Luminous 
{
  
  class GLSLShaderObject
  {
  public:
    GLSLShaderObject(GLenum type);
    ~GLSLShaderObject();

    bool compile();
    const char* compilerLog();   

    void setSource(const char* code);

  protected:
    // Disabled
    GLSLShaderObject(const GLSLShaderObject &) {}
    GLSLShaderObject & operator = (const GLSLShaderObject &) { return * this; }

    char* m_compilerLog;
    bool m_isCompiled;
    GLchar* m_shaderSource;
    GLuint m_handle;

    friend class GLSLProgramObject;
  };

}

#endif

