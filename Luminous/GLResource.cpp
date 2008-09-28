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

#include "GLResource.hpp"
#include "GLResources.hpp"
#include <cassert>

namespace Luminous
{

  GLResource::GLResource(GLResources * resources)
    : m_resources(resources),
      m_deleteOnFrame(0)
  {}
  
  GLResource::~GLResource()
  {}

  void GLResource::setResources(GLResources * resources)
  {
    assert(m_resources == 0 || resources == m_resources);
    m_resources = resources;
  }

  long GLResource::consumesBytes()
  {
    return 0;
  }

  void GLResource::changeByteConsumption(long deallocated, long allocated)
  {
    if(m_resources)
      m_resources->changeByteConsumption(deallocated, allocated);
  }

}


