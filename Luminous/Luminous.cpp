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

#include <Luminous/Luminous.hpp>
#include <Radiant/Trace.hpp>
#include <string>
#include <Magick++.h>

namespace Luminous
{

  bool initLuminous()
  {
    GLenum err = glewInit();

    if(err != GLEW_OK) { 
      Radiant::error("Failed to initialize GLEW: %s", glewGetErrorString(err));
      return false;
    }

    std::string versionMsg = "Luminous initialized: ";

    // Print which OpenGL version we have support for
    if(GLEW_VERSION_2_0)      versionMsg += std::string("OpenGL 2.0 supported");
    else if(GLEW_VERSION_1_5) versionMsg += std::string("OpenGL 1.5 supported");
    else if(GLEW_VERSION_1_4) versionMsg += std::string("OpenGL 1.4 supported");
    else if(GLEW_VERSION_1_3) versionMsg += std::string("OpenGL 1.3 supported");
    else if(GLEW_VERSION_1_2) versionMsg += std::string("OpenGL 1.2 supported");
    else if(GLEW_VERSION_1_1) versionMsg += std::string("OpenGL 1.1 supported");

//    versionMsg += std::string(" with GLSL version ") + std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    char * glsl = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::string glslMsg = (glsl ? glsl : "GLSL not supported");

    Radiant::trace("%s (%s)", versionMsg.c_str(), glslMsg.c_str());

    Magick::InitializeMagick(0);

    return true;
  }

}
