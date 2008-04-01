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

#ifndef LUMINOUS_DYNAMICTEXTURE_HPP
#define LUMINOUS_DYNAMICTEXTURE_HPP

#include <Luminous/Texture.hpp>
#include <Luminous/GLResource.hpp>
#include <Luminous/ImagePyramid.hpp>

#include <Radiant/RefPtr.hpp>

#include <Nimble/Vector2.hpp>

#include <vector>

namespace Luminous
{

  class DynamicTexture : public GLResource
  {
    public:
      DynamicTexture(GLResources * resources = 0);
      virtual ~DynamicTexture();

      virtual Texture2D * selectMipmap(); 

      virtual int mipmapsOnGPU() const;
      virtual int mipmapsOnCPU() const;

      virtual void updateGPUMipmaps();

      virtual void hint(const Nimble::Vector2 & size);
  
      ImagePyramid * pyramid() { return m_pyramid; }
      void setPyramid(ImagePyramid * pyramid) { m_pyramid = pyramid; }

      Texture2D * getMipmap(int n);

      float aspect(int level = 0) const;

    protected:
      std::vector<Radiant::RefPtr<Texture2D> > m_mipmaps;

      ImagePyramid * m_pyramid; 

      Nimble::Vector2i m_desiredSize;
  };

}

#endif
