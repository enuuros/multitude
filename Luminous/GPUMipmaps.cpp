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

#include "CPUMipmaps.hpp"

#include "GPUMipmaps.hpp"

#include <Luminous/GLResources.hpp>

#include <Radiant/Trace.hpp>

#include <assert.h>

namespace Luminous {

  using namespace Nimble;
  using namespace Radiant;

  GPUMipmaps::GPUMipmaps(CPUMipmaps * cpumaps, GLResources * resources)
    : GLResource(resources),
      m_cpumaps(cpumaps)
  {
    /* for(int i = 0; i < CPUMipmaps::MAX_MAPS; i++) {
      m_textures[i].setResources(resources);
    }
    */
  }

  GPUMipmaps::~GPUMipmaps()
  {}

  bool GPUMipmaps::bind(Nimble::Vector2 pixelsize)
  {
    int best = m_cpumaps->getOptimal(pixelsize);

    if(best < 0) {
      // trace("GPUMipmaps::bind # No mipmap");
      return false;
    }

    // trace("GPUMipmaps::bind %f %f %d", pixelsize.x, pixelsize.y, best);


    void * key = m_keys + best;

    GLResource * res = resources()->getResource(key);
    
    Texture2D * tex = 0;

    if(res) {
      // trace("Got optimal texture");
      m_cpumaps->markImage(best);
      tex = dynamic_cast<Texture2D *> (res);
      assert(tex);
      tex->bind();
    }
    else {

      int mybest = -1;

      // Try to find a ready texture object already in GPU RAM

      for(int i = 0; i < m_cpumaps->maxLevel() && !res; i++) {
        int index = best + i;

        if(index <= m_cpumaps->maxLevel()) {

          res = resources()->getResource(m_keys + index);
          if(res) {
            mybest = index;
          }
        }

        index = best - i;

        if(index >= 0 && !res) {
          res = resources()->getResource(m_keys + index);
          if(res) {
            mybest = index;
          }
        }
      }
      
      // See what is in CPU RAM

      int closest = m_cpumaps->getClosest(pixelsize);

      if(((closest < 0) || Math::Abs(mybest - best) <= Math::Abs(closest - best))
         && mybest >= 0) {
        // trace("MYTEX dist = %d", mybest - best);

        tex = dynamic_cast<Texture2D *> (res);
        assert(tex);
        tex->bind();
      }
      else if(closest >= 0) {
	// trace("CPUTEX dist = %d", closest - best);

        tex = new Texture2D(resources());
        resources()->addResource(m_keys + closest, tex);
        tex->loadImage(*m_cpumaps->getImage(closest), false);
        tex->bind();
      }      
    }
    
    if(tex) {
      // trace("Using TEX with %dx%d %d", tex->width(), tex->height(), best);
      resources()->deleteAfter(tex, 10);
    }
    else
      return false;

    return true;
  }

  bool GPUMipmaps::bind(const Nimble::Matrix3 & transform, 
			Nimble::Vector2 pixelsize)
  {
    return bind(transform.extractScale() * pixelsize);
  }

}
