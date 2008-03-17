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

#include "AudioLoop.hpp"

#include <Radiant/Trace.hpp>

#include <assert.h>
#include <strings.h>

#define FRAMES_PER_BUFFER 128

namespace Resonant {

  using Radiant::error;

  AudioLoop::AudioLoop()
    : m_stream(0),
      m_streamInfo(0),
      m_startTime(0),
      m_isRunning(false)
  {
    init();

    bzero( & m_inParams,  sizeof(m_inParams));
    bzero( & m_outParams, sizeof(m_outParams));
  }

  AudioLoop::~AudioLoop()
  {
    assert(isRunning() == false);
  }

  bool AudioLoop::init()
  {
    static bool once = false;

    if(once)
      return true;

    once = true;

    PaError e = Pa_Initialize();
    if(e != paNoError) {
      error("AudioLoop::init # %s", Pa_GetErrorText(e));
      return false;
    }
    return true;
  }

  bool AudioLoop::cleanup()
  {
    PaError e = Pa_Terminate();
    if(e != paNoError) {
      error("AudioLoop::cleanup # %s", Pa_GetErrorText(e));
      return false;
    }
    return true;
  }

  bool AudioLoop::startReadWrite(int samplerate,
				 int channels)
  {
    assert(!isRunning());
    
    m_stream = 0;
    m_streamInfo = 0;
    PaError err;

    bzero( & m_inParams,  sizeof(m_inParams));
    bzero( & m_outParams, sizeof(m_outParams));

    m_outParams.device = Pa_GetDefaultOutputDevice();
    m_outParams.channelCount = channels;
    m_outParams.sampleFormat = paFloat32;
    m_outParams.suggestedLatency = 
      Pa_GetDeviceInfo( m_outParams.device )->defaultLowOutputLatency;
    m_outParams.hostApiSpecificStreamInfo = 0;

    m_inParams = m_outParams;
    m_inParams.device = Pa_GetDefaultInputDevice();

    m_continue = true;

    err = Pa_OpenStream(& m_stream,
			0, // & m_inParams,
                        & m_outParams,
			samplerate,
			FRAMES_PER_BUFFER,
			paClipOff,
			paCallback,
			this );

    if( err != paNoError ) {
      Radiant::error("AudioLoop::startReadWrite # Pa_OpenStream failed");
      return false;
    }
    
    err = Pa_SetStreamFinishedCallback(m_stream, & paFinished );

    m_streamInfo = Pa_GetStreamInfo(m_stream);

    err = Pa_StartStream(m_stream);
    
    if( err != paNoError ) {
      Radiant::error("AudioLoop::startReadWrite # Pa_StartStream failed");
      return false;
    }

    m_startTime = Pa_GetStreamTime(m_stream);

    m_isRunning = true;

    Radiant::trace("AudioLoop::startReadWrite # lt = %lf, EXIT OK",
		   m_streamInfo->outputLatency);

    return true;
  }
  
  bool AudioLoop::stop()
  {
    if(!isRunning())
      return true;

    m_continue = false;

    int i = 0;
    
    int err = Pa_StopStream(m_stream);
    if(err != paNoError) {
      error("AudioLoop::stop # Could not stop the stream");

      while(isRunning() && i < 100) {
        Pa_Sleep(5);
        i++;
      }
    }
    else
      m_isRunning = false;

    err = Pa_CloseStream(m_stream);
    if(err != paNoError) {
      error("AudioLoop::stop # Could not close stream");
    }
    
    m_stream = 0;
    m_streamInfo = 0;

    return true;
  }

  int AudioLoop::paCallback(const void *in, void *out,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo* time,
			    PaStreamCallbackFlags status,
			    void * self)
  {
    AudioLoop * au = (AudioLoop *) self;

    int r = au->callback(in, out, framesPerBuffer, time, status);

    return au->m_continue ? r : paComplete;
  }

  void AudioLoop::paFinished(void * self)
  {
    ((AudioLoop *) self)->finished();
    // Radiant::trace("AudioLoop::paFinished # %p", self);
  }

  
  void AudioLoop::finished()
  {
    m_isRunning = false;
  }

}
