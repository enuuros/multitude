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


#ifndef LUMINOUS_ENABLE_STEP_HPP
#define LUMINOUS_ENABLE_STEP_HPP

#include <Patterns/NotCopyable.hpp>

#include <GL/glew.h>

namespace Luminous {

  class EnableStep : public Patterns::NotCopyable
  {
  public:
    EnableStep(GLenum feature) : m_feature(feature) { glEnable(feature); }
    ~EnableStep() { glDisable(m_feature); }
  private:
    GLenum m_feature;
  };

}

#endif
