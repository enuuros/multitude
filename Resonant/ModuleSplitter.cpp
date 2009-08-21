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

#include "ModuleSplitter.hpp"

#include <Radiant/BinaryData.hpp>
#include <Radiant/Trace.hpp>

#include <assert.h>
#include <string.h>
#include <strings.h>

namespace Resonant {

  using namespace Radiant;

  ModuleSplitter::ModuleSplitter(Application * a)
    : Module(a),
      m_outChannels(8)
  {}

  ModuleSplitter::~ModuleSplitter()
  {}

  bool ModuleSplitter::prepare(int & channelsIn, int & channelsOut)
  {
    (void) channelsIn;

    channelsOut = m_outChannels;

    return true;
  }
  
  void ModuleSplitter::control(const char * id, Radiant::BinaryData * data)
  {
    info("ModuleSplitter::control # %s", id);
    
    bool ok = true;

    if(strcmp(id, "addsource") == 0) {
      
    }
    else if(strcmp(id, "removeource") == 0) {
      
    }
    else if(strcmp(id, "setsourcelocation") == 0) {
      unsigned index = data->readInt32( & ok);
      Nimble::Vector2 loc = data->readVector2Float32( & ok);

      if(ok && index < m_sources.size()) {
	setSourceLocation(index, loc);
      }
    }
    else {
      ;
    }
  }

  void ModuleSplitter::process(float ** in, float ** out, int n)
  {
    int bufferbytes = n * sizeof(float);

    // Zero the output channels
    for(int i = 0; i < m_outChannels; i++) {
      bzero(out[i], bufferbytes);
    }

    for(int i = 0; i < (int) m_sources.size(); i++) {

      Source & s = *m_sources[i];

      for(int j = 0; j < PIPES_PER_SOURCE; j++) {

	Pipe & p = s.m_pipes[j];

	const float * src = in[i];
      	float * dest = out[p.m_to];
	float * sentinel = dest + n;

	if(p.m_ramp.left()) {

	  for( ; dest < sentinel; dest++, src++) {
	    *dest += (*src * p.m_ramp.value());
	    p.m_ramp.update();
	  }
	}
	else {
	  float v = p.m_ramp.value();
	  for( ; dest < sentinel; dest++, src++) {
	    *dest += (*src * v);
	  }
	}
      }
    }
    
  }

  void ModuleSplitter::setSourceLocation(unsigned index,
					 Nimble::Vector2 location)
  {
    assert(index < m_sources.size());

    Source & s = * m_sources[index];

    s.m_location = location;

    int interpSamples = 4000;
    
    // Make all pipes go towards zero:
    for(unsigned i = 0; i < PIPES_PER_SOURCE; i++) {
      Pipe & p = s.m_pipes[i];
      if(!p.done() && p.m_ramp.target() != 0.0f) {
	p.m_ramp.setTarget(0, interpSamples);
      }
    }

    for(unsigned i = 0; i < m_speakers.size(); i++) {
      LoudSpeaker & ls = m_speakers[i];
      float d = (location - ls.m_location).length();
      float rel = d / m_maxRadius;

      float inv = 1.0f - rel;
      float gain = Nimble::Math::Min(inv * 2.0f, 1.0f);

      if(gain <= 0.0000001f) {
	// Silence that output:
	for(unsigned j = 0; j < PIPES_PER_SOURCE; j++) {
	  Pipe & p = s.m_pipes[j];
	  if(p.m_to == i && p.m_ramp.target() > 0.0001f) {
	    p.m_ramp.setTarget(0.0f, interpSamples);
	  }
	}
      }
      else {
	bool found = false;

	// Find existing pipe:
	for(unsigned j = 0; j < PIPES_PER_SOURCE && !found; j++) {
	  Pipe & p = s.m_pipes[j];
	  if(p.m_to == i && p.m_ramp.target() > 0.0001f) {
	    p.m_ramp.setTarget(gain, interpSamples);
	    found = true;
	  }
	}

	if(!found) {
	  
	  // Pick up a new pipe:
	  for(unsigned j = 0; j < PIPES_PER_SOURCE && !found; j++) {
	    Pipe & p = s.m_pipes[j];
	    if(p.done()) {
	      p.m_to = i;
	      p.m_ramp.setTarget(gain, interpSamples);
	      found = true;
	    }
	  }
	}

	if(!found) {
	  error("Could not allocate pipe for a moving source %u", index);
	}
      }
    }
  }

}
