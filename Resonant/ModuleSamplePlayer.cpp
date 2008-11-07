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

#include "ModuleSamplePlayer.hpp"

#include "ControlData.hpp"

#include <Nimble/Math.hpp>

#include <Radiant/FileUtils.hpp>
#include <Radiant/Trace.hpp>

#include <assert.h>

#ifdef WIN32
#include <WinPort.h>
#endif

namespace Resonant {

  using namespace Radiant;

  ModuleSamplePlayer::Sample::Sample()
  {
    bzero(&m_info, sizeof(m_info));
  }

//  ModuleSamplePlayer::Sample::~Sample()
//  {}

  bool ModuleSamplePlayer::Sample::load(const char * filename,
					const char * name)
  {
    if(!Radiant::FileUtils::fileReadable(filename))
      return false;
    
    m_name = name;

    bzero(&m_info, sizeof(m_info));

    SNDFILE * sndf = sf_open(filename, SFM_READ, & m_info);

    if(!sndf)
      return false;

    m_data.resize(m_info.channels * m_info.frames);
    if(!m_data.empty())
      bzero( & m_data[0], m_data.size() * sizeof(float));
    
    uint block = 1000;

    unsigned pos = 0;

    while(pos < m_info.frames) {
      uint get = Nimble::Math::Min((uint) (m_info.frames - pos), block);
      uint n = get * m_info.channels;
      
      sf_read_float(sndf, & m_data[pos * m_info.channels], n);

      pos += get;
    }

    sf_close(sndf);

    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool ModuleSamplePlayer::SampleVoice::synthesize(float ** out, int n)
  {
    if(m_state != PLAYING) {
      //printf(":"); fflush(0);
      return m_state == WAITING_FOR_SAMPLE;
    }
    
    // printf("+"); fflush(0);
   
    unsigned avail = m_sample->data().size() - m_position;

    if((int) avail > n)
      avail = n;

    float * b1 = out[0];
    float gain = m_gain;
    float pitch = m_relPitch;

    bool more;

    if(pitch == 1.0f) {
      const float * src = m_sample->buf(m_position);
    
      for(unsigned i = 0; i < avail; i++) {
	*b1++ += *src++ * gain;
	
      }

      m_position += avail;

      more = (int) avail == n;
    }
    else {
      double dpos = m_dpos;
      double dmax = m_sample->data().size() - 1;

      const float * src = m_sample->buf(0);

      for(int i = 0 ; i < n && dpos < dmax; i++) {

	int base = (int) dpos;
	double w2 = dpos - (double) base;
	*b1++ = float((src[base] * (1.0 - w2) + src[base+1] * w2) * gain);
	dpos += pitch;
      }

      m_dpos = dpos;

      more = dpos < dmax;
    }
    

    if(!more) {
      m_sample = 0;
      m_state = INACTIVE;
    }

    return more != 0;
  }

  void ModuleSamplePlayer::SampleVoice::init
  (Sample * sample, ControlData * data)
  {
    m_sample = sample;
    m_position = 0;
    bool ok = true;

    float gain = data->readFloat32( & ok);

    if(ok)
      m_gain = gain;
    else
      m_gain = 1.0f;

    float pitch = data->readFloat32( & ok);

    if(ok)
      m_relPitch = pitch;
    else
      m_relPitch = 1.0f;

    m_dpos = 0.0;

    m_state = sample ? PLAYING : WAITING_FOR_SAMPLE;
  }

  void ModuleSamplePlayer::SampleVoice::setSample(Sample * s)
  {
    assert(m_state == WAITING_FOR_SAMPLE);
    m_sample = s;
    m_state = PLAYING;
  }


  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ModuleSamplePlayer::LoadItem::LoadItem()
    : m_free(true)
  {
    bzero(m_waiting, sizeof(m_waiting));
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ModuleSamplePlayer::BGLoader::BGLoader(ModuleSamplePlayer * host)
    : m_host(host)
  {
    m_continue = true;
    run();
  }

  ModuleSamplePlayer::BGLoader::~BGLoader()
  {
    if(isRunning()) {
      m_continue = false;
      m_cond.wakeOne(m_mutex);
      waitEnd();
    }
  }

  bool ModuleSamplePlayer::BGLoader::addLoadable(const char * filename,
						 SampleVoice * waiting)
  {
    trace(DEBUG, "ModuleSamplePlayer::BGLoader::addLoadable # %s %p",
	  filename, waiting);

    for(int i = 0; i < BINS; i++) {
      if(m_loads[i].m_name == filename) {
	return m_loads[i].addWaiting(waiting);
      }
    }

    for(int i = 0; i < BINS; i++) {
      if(m_loads[i].m_free) {
	m_loads[i].m_name = filename;
	m_loads[i].addWaiting(waiting);
	m_loads[i].m_free = false;
	break;
      }
    }
    
    m_cond.wakeAll(m_mutex);

    return false;
  }

  void ModuleSamplePlayer::BGLoader::childLoop()
  {
    while(m_continue) {

      trace(DEBUG, "ModuleSamplePlayer::BGLoader::childLoop # once");

      for(int i = 0; i < BINS; i++) {
	LoadItem & it = m_loads[i];
	
	if(!it.m_free) {

	  trace(DEBUG, "ModuleSamplePlayer::BGLoader::childLoop # Something");

	  Sample * s = new Sample();

	  bool good = true;

	  if(!s->load(it.m_name.str(), it.m_name.str())) {
	    error("ModuleSamplePlayer::BGLoader::childLoop # Could not load "
		  "\"%s\"", it.m_name.str());
	    good = false;
	  }
	  else if(!m_host->addSample(s)) {
	    error("ModuleSamplePlayer::BGLoader::childLoop # Could not add "
		  "\"%s\"", it.m_name.str());
	    good = false;
	  }

	  if(!good) {
	    delete s;
	    for(int j = 0; j < LoadItem::WAITING_COUNT; i++) {
	      SampleVoice * voice = it.m_waiting[i];
	      if(!voice)
		break;
	      voice->loadFailed();
	    }
	  }
	  else {
	    trace(DEBUG, "ModuleSamplePlayer::BGLoader::childLoop # Loaded "
		  "\"%s\"", it.m_name.str());

	    for(int j = 0; j < LoadItem::WAITING_COUNT; j++) {
	      SampleVoice * voice = it.m_waiting[j];
	      if(!voice)
		break;

	      trace(DEBUG, "ModuleSamplePlayer::BGLoader::childLoop # Delivering "
		    "\"%s\"", it.m_name.str());
	      
	      voice->setSample(s);
	    }
	  }
	  
	  it.m_free = true;
	}
      }

      m_mutex.lock();
      m_cond.wait(m_mutex);
      m_mutex.unlock();
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ModuleSamplePlayer::ModuleSamplePlayer(Application * a)
    : Module(a),
      m_channels(1),
      m_active(0)
//      m_loader(this)
  {
    m_voices.resize(256);
    m_voiceptrs.resize(m_voices.size());
    if(!m_voiceptrs.empty())
      bzero( & m_voiceptrs[0], m_voiceptrs.size() * sizeof(SampleVoice *));
    m_samples.resize(2048);

    m_loader = new BGLoader(this);
  }

  ModuleSamplePlayer::~ModuleSamplePlayer()
  {}

  bool ModuleSamplePlayer::prepare(int & channelsIn, int & channelsOut)
  {
    bool edit = false;

    if(channelsIn) {
      edit = true;
      channelsIn = 0;
    }

    if(channelsOut != (int) m_channels) {
      edit = true;
      channelsOut = m_channels;
    }

    return true;
  }

  void ModuleSamplePlayer::control(const char * id, ControlData * data)
  {
    const int bufsize = 256;
    char buf[bufsize];

    if(strcmp(id, "playsample") == 0) {
      int voiceind = findFreeVoice();

      if(voiceind < 0) {
        error("ModuleSamplePlayer::control # Out of polyphony");
        return;
      }

      bool ok = data->readString(buf, bufsize);

      if(!ok) {
        error("ModuleSamplePlayer::control # Could not get sample name ");
        return;
      }

      SampleVoice & voice = m_voices[voiceind];
      m_voiceptrs[m_active] = & voice;
      
      int sampleind = findSample(buf);

      if(sampleind < 0) {
		  Radiant::trace(Radiant::DEBUG, "ModuleSamplePlayer::control # No sample \"%s\"", buf);
	
	m_loader->addLoadable(buf, & voice);

        // return;
      }

      voice.init(sampleind >= 0 ? 
		 m_samples[sampleind].ptr() : 0, data);
      m_active++;

      trace(DEBUG, "ModuleSamplePlayer::control # Started sample %s (%d/%d)",
	    buf, voiceind, m_active);
      // assert(voiceind < (int) m_active);

    }
    else
      error("ModuleSamplePlayer::control # Unknown message \"%s\"", id);
  }

  void ModuleSamplePlayer::process(float ** , float ** out, int n)
  {
    uint i;

    // First zero the outputs
    for(i = 0; i < m_channels; i++)
      bzero(out[i], n * 4);

    // Then fill the outputs with audio
    for(i = 0; i < m_active; ) {
      if(!m_voiceptrs[i]->synthesize(out, n))
	dropVoice(i);
      else
	i++;
    }
  }

  bool ModuleSamplePlayer::addSample(const char * filename, const char * name)
  {
    if(!Radiant::FileUtils::fileReadable(filename))
      return false;

    SampleInfo sv;
    sv.m_name = name;
    sv.m_filename = filename;
    m_sampleList.push_back(sv);

    return true;
  }

  int ModuleSamplePlayer::findFreeVoice()
  {
    for(unsigned i = 0; i < m_voices.size(); i++) {
      if(!m_voices[i].isActive())
        return i;
    }
    return -1;
  }

  int ModuleSamplePlayer::findSample(const char * name)
  {
    for(unsigned i = 0; i < m_samples.size(); i++) {
      Sample * s = m_samples[i].ptr();

      if(s)
	if(s->name() == name)
	  return i;
    }

    return -1;
  }

  void ModuleSamplePlayer::loadSamples()
  {
    m_samples.clear();

    for(std::list<SampleInfo>::iterator it = m_sampleList.begin();
	it != m_sampleList.end(); it++) {
      Radiant::RefPtr<Sample> s(new Sample());
      if(s->load((*it).m_filename.c_str(), (*it).m_name.c_str()))
	m_samples.push_back(s);
    }
  }

  bool ModuleSamplePlayer::addSample(Sample * s)
  {
    for(unsigned i = 0; i < m_samples.size(); i++) {
      if(!m_samples[i].ptr()) {
	m_samples[i] = s;
	return true;
      }
      trace(DEBUG, "ModuleSamplePlayer::addSample # m_samples[%u] = %p",
	    i, m_samples[i].ptr());
    }

    return false;
  }
    
  void ModuleSamplePlayer::dropVoice(unsigned i)
  {
    // trace("ModuleSamplePlayer::dropVoice # %d", i);
    assert((uint) i < m_active);
    m_active--;
    m_voiceptrs[i]->clear();
    for( ; i < m_active; i++) {
      m_voiceptrs[i] = m_voiceptrs[i + 1];
    }
    m_voiceptrs[m_active] = 0;
  }
}

