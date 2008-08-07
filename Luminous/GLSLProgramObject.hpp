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

#ifndef LUMINOUS_GLSLPROGRAMOBJECT_HPP
#define LUMINOUS_GLSLPROGRAMOBJECT_HPP

#include <Luminous/GLResource.hpp>
#include <Luminous/GLSLShaderObject.hpp>

#include <Patterns/NotCopyable.hpp>

#include <list>
#include <string>
#include <vector>

namespace Luminous 
{

  /// OpenGL shading language program object
  class MTEXPORT GLSLProgramObject : public GLResource, public Patterns::NotCopyable
  {
  public:
    GLSLProgramObject(GLResources * resources = 0);
    virtual ~GLSLProgramObject();
    
    void addObject(GLSLShaderObject* obj);

    virtual bool link();
    virtual void clear();

    const char * linkerLog();
    
    virtual void bind();
    virtual void unbind();
    
    int getUniformLoc(const std::string & name);
    int getUniformLoc(const char * name);

    int getAttribLoc(const std::string & name);
    int getAttribLoc(const char * name);
    
    bool validate();
    
    static GLSLProgramObject* fromFiles
    (const char* vsFile, const char* fsFile);

    int shaderObjectCount() const { return m_shaderObjects.size(); } 

    GLuint handle() const { return m_handle; }

  protected:
    
    std::vector<GLchar> m_linkerLog;
    bool m_isLinked;
    std::list<GLSLShaderObject*> m_shaderObjects;
    GLuint m_handle;

  };

}

#endif

