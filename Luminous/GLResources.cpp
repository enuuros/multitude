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

#include "GLResources.hpp"
#include "GarbageCollector.hpp"
#include "GLResource.hpp"

#include <Nimble/Math.hpp>

#include <Radiant/Mutex.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/Trace.hpp>

#include <cassert>

#include <stdlib.h>

namespace Luminous
{
  
  using namespace Radiant;

  GLResources::GLResources(Radiant::ResourceLocator & rl)
    : m_deallocationSum(0),
      m_allocationSum(0),
      m_consumingBytes(0),
      m_comfortableGPURAM((1 << 20) * 70), // 70 MB
      m_frame(0),
      m_resourceLocator(rl)
  {
    const char * envgp = getenv("MULTI_GPU_RAM");

    if(envgp) {

      m_comfortableGPURAM = Nimble::Math::Max(atol(envgp) * (1 << 20),
					      m_comfortableGPURAM);
    }
  }

  GLResources::~GLResources()
  {
    while(m_resources.size())
      eraseResource((*m_resources.begin()).first);

    if(m_consumingBytes != 0)
      Radiant::error("GLResources::~GLResources # The GPU memory is left at %ld -> "
                     "there is a bug in your application.",
                     m_consumingBytes);
  }

  GLResource * GLResources::getResource(const Collectable * key)
  {
    iterator it = m_resources.find(key);

    if(it == m_resources.end())
      return 0;

    return (*it).second;
  }

  void GLResources::addResource(const Collectable * key, GLResource * resource)
  {
    iterator it = m_resources.find(key);

    if(it != m_resources.end()) {
      Radiant::error("GLResources::addResource # There already is a resource for %p", key);
      eraseResource(key);
    }

    m_resources[key] = resource;
    long bytes = resource->consumesBytes();
    m_consumingBytes += bytes;
    m_allocationSum += bytes;
  }

  bool GLResources::eraseResource(const Collectable * key)
  {
    iterator it = m_resources.find(key);

    if(it == m_resources.end()) {
      //Radiant::error("GLResources::eraseResource # No resource for %p", key);
      return false;
    }

    GLResource * resource = (*it).second;

    /* long bytes = resource->consumesBytes();

    m_consumingBytes  -= bytes;
    m_deallocationSum += bytes;
    */
    delete resource;
    m_resources.erase(it);

//    Radiant::trace("GLResources::eraseResource # Resource erased.");

    return true;
  }

  void GLResources::eraseResources()
  {
    for(GarbageCollector::iterator it = GarbageCollector::begin();
        it != GarbageCollector::end(); it++) {
      const Collectable * key = GarbageCollector::getObject(it);
      eraseResource(key);
    }

    m_frame++;
    
    for(iterator it = m_resources.begin();
	(m_consumingBytes >= m_comfortableGPURAM) && 
	  (it != m_resources.end()); ) {
      
      GLResource * r = (*it).second;

      if(r->m_deleteOnFrame && r->m_deleteOnFrame < m_frame) {
	
	// Radiant::trace("GLResources::eraseResources # Removing old");

        delete (*it).second;
        iterator tmp = it;
        tmp++;
        m_resources.erase(it);
        it = tmp;
      }
      else
        it++;
    }
  }

  void GLResources::clear()
  {
    while(m_resources.size()) {
      GLResource * res = (*m_resources.begin()).second;
      delete res;
      m_resources.erase(m_resources.begin());
    }

    m_deallocationSum = 0;
    m_allocationSum = 0;
    m_consumingBytes = 0;
  }

  void GLResources::changeByteConsumption(long deallocated, long allocated)
  {
    m_deallocationSum += deallocated;
    m_allocationSum   += allocated;
    m_consumingBytes  += (allocated - deallocated);

    assert(m_consumingBytes >= 0);
  }

  void GLResources::deleteAfter(GLResource * resource, int frames)
  {
    if(frames >= 0)
      resource->m_deleteOnFrame = m_frame + frames;
    else
      resource->m_deleteOnFrame = -1;
  }

  typedef std::map<Thread::id_t, GLResources *> ResourceMap;
  static ResourceMap __resources;
  static MutexStatic __mutex;
  
  void GLResources::setThreadResources(GLResources * rsc)
  {
    GuardStatic g(&__mutex);
    __resources[Thread::myThreadId()] = rsc;
  }

  GLResources * GLResources::getThreadResources()
  {
    GuardStatic g(&__mutex);
    
    ResourceMap::iterator it = __resources.find(Thread::myThreadId());
    
    if(it == __resources.end()) {
      error("No resources for current thread");
      return 0;
    }
    
    return (*it).second;
  }

}
