/* COPYRIGHT
 *
 * This file is part of VideoDisplay.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "VideoDisplay.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "AudioTransfer.hpp"
#include "VideoIn.hpp"

#include <Radiant/Trace.hpp>

namespace VideoDisplay {

  AudioTransfer::AudioTransfer(Resonant::Application * a, VideoIn * video)
    : Module(a),
      m_video(video),
      m_channels(0),
      m_started(false),
      m_stopped(false),
      m_sampleFmt(Radiant::ASF_INT16),
      m_frames(0)
  {
    Radiant::trace(Radiant::DEBUG, "AudioTransfer::AudioTransfer # %p", this);
  }

  AudioTransfer::~AudioTransfer()
  {
    Radiant::trace(Radiant::DEBUG, "AudioTransfer::~AudioTransfer # %p", this);
  }

  bool AudioTransfer::prepare(int & channelsIn, int & channelsOut)
  {
    Radiant::trace(Radiant::DEBUG, "AudioTransfer::prepare");
    
    if(!m_video) {
      Radiant::trace(Radiant::FAILURE, "AudioTransfer::prepare # No video source");
      m_stopped = true;
      return false;
    }

    m_channels = 0;
    m_sampleFmt = Radiant::ASF_INT16;
    int sr = 44100;
    
    m_video->getAudioParameters( & m_channels, & sr, & m_sampleFmt);

    channelsIn = 0;
    channelsOut = m_channels;

    m_frames = 0;
    m_started = true;
    m_stopped = false;
    
    return true;
  }

  void AudioTransfer::process(float **, float ** out, int n)
  {
    // Radiant::trace("AudioTransfer::process");

    int m = n;
    const void * audio = m_video->getAudio( & m, false);
 
    int chans = m_channels;

    /* if(m)
      Radiant::info("AudioTransfer::process # %s %ld + %d", 
		    m_video->name(), m_frames, m);
    */
    if(m_sampleFmt == Radiant::ASF_INT16) {

      const int16_t * data = (const int16_t *) audio;
      const float scale = 0.7f / 32768.0f;

      for(int i = 0; i < m_channels; i++) {
	const int16_t * src = data + i;
	float * dest = out[i];
	
	int k;
	
	for(k = 0; k < m; k++)
	  dest[k] = src[k * chans] * scale;
	
	// Cleanup:
	for(; k < n; k++)
	  dest[k] = 0.0f;
      }
    }
    else
      Radiant::error("AudioTransfer::process # Unsupported sample format");

    m_frames += m;
  }

  bool AudioTransfer::stop()
  {
    if(m_video)
      m_video->stopDecoding();
    m_stopped = true;
    return true;
  }

  Radiant::TimeStamp AudioTransfer::audioTime()
  {
    int foo, sr = 44100;
    Radiant::AudioSampleFormat foo2;

    if(m_video)
      m_video->getAudioParameters(&foo, & sr, & foo2);

    double secs = m_frames / (double) sr - 0.1;

    return Radiant::TimeStamp::createSecondsD(secs);

    // Radiant::FAILURE("AudioTransfer::audioTime # UNIMPLEMENTED");
    // return 0;
  }

    
  

}
