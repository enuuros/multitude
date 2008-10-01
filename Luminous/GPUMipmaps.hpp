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

#ifndef LUMINOUS_GPU_MIPMAPS_HPP
#define LUMINOUS_GPU_MIPMAPS_HPP

#include <Luminous/Export.hpp>
#include <Luminous/GLResource.hpp>
#include <Luminous/Texture.hpp>

#include <Nimble/Matrix3.hpp>

#include <Radiant/RefPtr.hpp>

#include "CPUMipmaps.hpp"

namespace Luminous {

  /** A set of image mipmaps in the GPU memory. This class is used to
      load images from the CPUMipmaps object to the GPU as
      required and available. */
  class LUMINOUS_API GPUMipmaps : public GLResource
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
