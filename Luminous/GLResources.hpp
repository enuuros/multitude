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

#ifndef LUMINOUS_GLRESOURCES_HPP
#define LUMINOUS_GLRESOURCES_HPP

#include <map>

namespace Luminous
{
  class GarbageCollector;
  class GLResource;

  /// Collection of OpenGL-context -specific resources (textures, FBOs etc).
  /** This class is also used to store information about how much
      resources are used (texture, FBO -memory etc).

      This object also keeps track of change history in
      allocation/deallocation sums. This information is handy when you
      want to make sure that you do not push too many texture pixels
      to the GPU during one frame etc. */
  class GLResources
  {
  public:

    typedef std::map<void *, GLResource *> container;
    typedef container::iterator iterator;

    GLResources();
    virtual ~GLResources();

    /// Get a handle to a resource 
    GLResource * getResource(void * key);
    /// Adds a resource
    void addResource(void * key, GLResource * resource);
    /// Erases a single GLResource
    bool eraseResource(void * key);
    /// Erase the resources that are no longer required
    void eraseResources(GarbageCollector * collector);
    /// Tell the resource manager that byte consumption was changed
    /** Individual resource objects should call this function when
      their byte consumption changes. A typical example might be a
      texture object that is resized, or gets new mipmaps. */
    void changeByteConsumption(long deallocated, long allocated);
    
    /// Total number of bytes used on the GPU
    long consumesBytes() const { return m_consumingBytes; }

    /// Number of bytes deallocated since last counter reset
    long deallocationSum() { return m_deallocationSum; }
    /// Number of bytes allocated since last counter reset
    long allocationSum()  { return m_allocationSum; }

    /// Resets the allocation/deallocation sum counters
    /** A typical place to call this function is right before
      rendering the OpenGL scene. */
    void resetSumCounters() { m_deallocationSum = m_allocationSum = 0; }

 protected:

    container m_resources;

    long m_deallocationSum;
    long m_allocationSum;
    /// The number of bytes that reside on th GPU
    /** This number is likely to be quite approximate as we cannot
	estimate exactly much GPU memory a particular object uses. */
    long m_consumingBytes;
  };
}

#endif
