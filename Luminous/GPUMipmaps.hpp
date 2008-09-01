/* COPYRIGHT
 *
 * This file is part of TimeLine.
 *
 * Copyright: MultiTouch Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2007-2008
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from MultiTouch Oy.
 *
 * See file "TimeLine.hpp" for authors and more details.
 *
 */

#ifndef LUMINOUS_GPU_MIPMAPS_HPP
#define LUMINOUS_GPU_MIPMAPS_HPP

#include <Luminous/GLResource.hpp>
#include <Luminous/Texture.hpp>

#include <Nimble/Matrix3.hpp>

#include <Radiant/RefPtr.hpp>

#include "CPUMipmaps.hpp"

namespace Luminous {

  class GPUMipmaps : public GLResource
  {
  public:
    GPUMipmaps(CPUMipmaps *, GLResources * resources = 0);
    virtual ~GPUMipmaps();

    bool bind(Nimble::Vector2 pixelsize);
    bool bind(const Nimble::Matrix3 & transform, 
	      Nimble::Vector2 pixelsize);

  private:
    CPUMipmaps * m_cpumaps;

    char m_keys[CPUMipmaps::MAX_MAPS];

    // Texture2D m_textures[];
  };

}

#endif
