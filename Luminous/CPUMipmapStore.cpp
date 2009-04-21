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

#include "CPUMipmapStore.hpp"

#include <Radiant/Trace.hpp>

namespace Luminous {
  
  using Radiant::debug;

  static Radiant::MutexStatic __mutex;

  /* Note that the mipmaps are not deleted opun application exit. This
     is done on purpose: As we are daeling with static data one easily
     gets conflicts with the order of deleting resources. As a result
     it is safest to not delete the resources. Anyhow, the operating
     system should be able to clear the resources as the application
     shuts down. */

  class MipmapItem
  {
  public:
    MipmapItem() : m_linkCount(0), m_mipmaps(0) {}

    void incrCount() { m_linkCount++; }
    void decrCount() 
    {
      m_linkCount--;
      if(m_linkCount == 0) {
	delete m_mipmaps;
	m_mipmaps = 0;
      }
    }

    int m_linkCount;
    CPUMipmaps * m_mipmaps;
  };

  static std::map<std::string, MipmapItem> __mipmaps;
  typedef std::map<std::string, MipmapItem> MipMapItemContainer;

  CPUMipmaps * CPUMipmapStore::acquire(const std::string & filename,
				       GarbageCollector * gc)
  {
    Radiant::GuardStatic g( & __mutex);
    
    MipMapItemContainer::iterator it = __mipmaps.find(filename);
    
    if(it !=  __mipmaps.end()) {
      MipmapItem & mmi = (*it).second;
      mmi.incrCount();
      return mmi.m_mipmaps;
    }

    CPUMipmaps * mipmaps = new CPUMipmaps(gc);

    if(!mipmaps->startLoading(filename.c_str(), true)) {
      delete mipmaps;
      return 0;
    }

    __mipmaps[filename].m_mipmaps = mipmaps;
    __mipmaps[filename].incrCount();

    debug("CPUMipmapStore::acquire # Created new for %s (%d links)",
	  filename.c_str(), __mipmaps[filename].m_linkCount);

    return mipmaps;
  }

  CPUMipmaps * CPUMipmapStore::acquire(const char * filename,
				       GarbageCollector * gc)
  {
    return acquire(std::string(filename), gc);
  }

  void CPUMipmapStore::release(CPUMipmaps * mipmaps)
  {
    Radiant::GuardStatic g( & __mutex);

    for(MipMapItemContainer::iterator it = __mipmaps.begin();
	it != __mipmaps.end(); it++) {
      MipmapItem & mmi = (*it).second;
      if(mmi.m_mipmaps == mipmaps) {
	mmi.decrCount();
	if(!mmi.m_linkCount) {
	  __mipmaps.erase(it);
	}
      }
    }
  }

}
