/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
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
#include <sstream>

namespace Luminous
{

  bool initLuminous()
  {
    GLenum err = glewInit();

	std::ostringstream versionMsg;

    if(err != GLEW_OK) { 
      Radiant::trace(Radiant::FAILURE, "Failed to initialize GLEW: %s", glewGetErrorString(err));
      return false;
    }

	// Check the OpenGL version
	bool warn = true;
    versionMsg << "Luminous initialized: ";
		
	if(GLEW_VERSION_2_1) {
		warn = false;
		versionMsg << "OpenGL 2.1 supported";
	} else if(GLEW_VERSION_2_0) {
		warn = false;
		versionMsg << "OpenGL 2.0 supported";
	} else if(GLEW_VERSION_1_5) versionMsg << std::string("OpenGL 1.5 supported");
    else if(GLEW_VERSION_1_4) versionMsg << std::string("OpenGL 1.4 supported");
    else if(GLEW_VERSION_1_3) versionMsg << std::string("OpenGL 1.3 supported");
    else if(GLEW_VERSION_1_2) versionMsg << std::string("OpenGL 1.2 supported");
    else if(GLEW_VERSION_1_1) versionMsg << std::string("OpenGL 1.1 supported");

    char * glsl = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::string glslMsg = (glsl ? glsl : "GLSL not supported");

    Radiant::trace(Radiant::INFO, "%s (%s)", versionMsg.str().c_str(), glslMsg.c_str());
    if(warn) { 
      Radiant::trace(Radiant::FAILURE, "OpenGL 2.0 is not supported by this computer, "
		     "some applications may fail.");
      return false;
    }

    return true;
  }

}
