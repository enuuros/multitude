/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Resonant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "ModuleOutCollect.hpp"

#include "ControlData.hpp"
#include "DSPNetwork.hpp"

#include <Nimble/Random.hpp>

#include <Radiant/Trace.hpp>

#include <algorithm>

#ifdef WIN32
#include <WinPort.h>
#endif

namespace Resonant {

  using Radiant::debug;
  using Radiant::error;
  
  ModuleOutCollect::ModuleOutCollect(Application * a , DSPNetwork * host)
    : Module(a),
      m_host(host)
  {}

  ModuleOutCollect::~ModuleOutCollect()
  {}

  bool ModuleOutCollect::prepare(int & channelsIn, int & channelsOut)
  {
    channelsOut = 0;

    channelsIn = m_map.size();
    m_channels = m_host->outChannels();

    assert(m_channels != 0);

    m_interleaved.resize(m_channels * MAX_CYCLE);

    Radiant::debug("ModuleOutCollect::prepare # %d", (int) m_channels);

    return true;
  }

  void ModuleOutCollect::control(const char * address, ControlData * control)
  {
    assert(address != 0);

    bool ok = true;
    Move tmp;
    
    ok = control->readString(tmp.sourceId, Module::MAX_ID_LENGTH);
    tmp.from = control->readInt32( & ok);
    tmp.to   = control->readInt32( & ok);

    Radiant::debug("ModuleOutCollect::control # %s", address);
    
    if(!ok) {
     error("ModuleOutCollect::control # Could not parse control # %s",
                     tmp.sourceId);
      return;
    }
    else if(strcmp(address, "newmapping") == 0) {
      m_map.push_back(tmp);
      debug("ModuleOutCollect::control # newmapping %s %d -> %d",
            tmp.sourceId, tmp.from, tmp.to);
    }
    else if(strcmp(address, "removemapping") == 0) {
      std::vector<Move>::iterator it =
	std::find(m_map.begin(), m_map.end(), tmp);

      if(it != m_map.end()) {
	m_map.erase(it);
      }
      else
	error("ModuleOutCollect::control # Could not erase mapping");
    }
    else {
     error("ModuleOutCollect::control # No param \"%s\"", address);
    }
  }

  void ModuleOutCollect::process(float ** in, float **, int n)
  {
    int chans = m_channels;

    assert((int) m_interleaved.size() >= (n * chans));

    // Set to zero
    if(!m_interleaved.empty())
      bzero( & m_interleaved[0], sizeof(float) * n * chans);

    for(uint i = 0; i < m_map.size(); i++) {

      int to = m_map[i].to;
      
      const float * src = in[i];
      const float * sentinel = src + n;

      if(!src)
        continue; // Should output a warning ;-)

      float * dest = & m_interleaved[to];

      while(src < sentinel) {
	*dest += * src;

	src++;
	dest += chans;
      }
    }

    /*
    static Nimble::RandomUniform __r;

    uint nn = chans * n;
    for(uint i = 0; i < nn; i++)
      m_interleaved[i] = __r.rand11();
    */
  }
    
    
}
