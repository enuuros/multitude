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

#ifndef RESONANT_AUDIO_LOOP_HPP
#define RESONANT_AUDIO_LOOP_HPP

#include <portaudio.h>

namespace Resonant {

  /** A simple audio IO class that uses PortAudio to handle the real work. */
  
  class EXPORT AudioLoop
  {
  public:
    AudioLoop();
    virtual ~AudioLoop();

    static bool init();
    static bool cleanup();
    
    bool startReadWrite(int samplerate,
			int channels);
    bool isRunning() { return m_isRunning; }

    bool stop();

    const PaStreamParameters & inParameters()  { return m_inParams; }
    const PaStreamParameters & outParameters() { return m_outParams; }

  protected:
    static int paCallback(const void *in, void *out,
			  unsigned long framesPerBuffer,
			  const PaStreamCallbackTimeInfo* time,
			  PaStreamCallbackFlags status,
			  void * self);
    static void paFinished(void * self);

    virtual int callback(const void *in, void *out,
			 unsigned long framesPerBuffer,
			 const PaStreamCallbackTimeInfo* time,
			 PaStreamCallbackFlags status) = 0;

    virtual void finished();

    PaStreamParameters m_inParams;
    PaStreamParameters m_outParams;

    PaStream * m_stream;
    const PaStreamInfo * m_streamInfo;
    PaTime     m_startTime;
    bool       m_isRunning;
    bool       m_continue;
  };

}


#endif

