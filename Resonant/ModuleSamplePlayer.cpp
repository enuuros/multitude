/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
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

  ModuleSamplePlayer::Sample::~Sample()
  {}

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
    bzero( & m_data[0], m_data.size() * sizeof(float));
    
    uint block = 10000;

    unsigned pos = 0;

    while(pos < m_info.frames) {
      uint get = Nimble::Math::Min((uint) (m_info.frames - pos), block);
      uint n = pos * m_info.channels;
      sf_read_float(sndf, & m_data[n], n);

      pos += get;
    }

    sf_close(sndf);

    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  bool ModuleSamplePlayer::SampleVoice::synthesize(float ** out, int n)
  {
    unsigned avail = m_sample->data().size() - m_position;

    if((int) avail > n)
      avail = n;

    float * b1 = out[0];
    
    const float * src = m_sample->buf(m_position);

    for(unsigned i = 0; i < avail; i++) {
      *b1++ = *src++;
    }

    m_position += avail;
    bool ok = (int) avail == n;

    if(!ok) {
      m_sample = 0;
    }

    return ok;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  ModuleSamplePlayer::ModuleSamplePlayer(Application * a)
    : Module(a),
      m_channels(1),
      m_active(0)
  {}

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
      
      int sampleind = findSample(buf);

      if(sampleind < 0) {
        error("ModuleSamplePlayer::control # No sample \"%s\"", buf);
        return;
      }

      voice.init(m_samples[sampleind].ptr());
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
    for(i = 0; i < m_voices.size(); ) {
      if(!m_voices[i].synthesize(out, n))
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
    for(unsigned i = 0; i < m_voices.size(); i++)
      if(!m_voices[i].isActive())
        return i;

    return -1;
  }

  int ModuleSamplePlayer::findSample(const char * name)
  {
    for(unsigned i = 0; i < m_samples.size(); i++)
      if(m_samples[i].ptr()->name() == name)
        return i;

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
    
  void ModuleSamplePlayer::dropVoice(unsigned i)
  {
    assert((uint) i < m_active);
    m_active--;
    for( ; i < m_active; i++) {
      m_voices[i] = m_voices[i + 1];
    }
  }
}

