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

#ifndef RESONANT_AUDIO_LOOP_HPP
#define RESONANT_AUDIO_LOOP_HPP

#include <Resonant/Export.hpp>

namespace Resonant {

  /** A simple audio IO class that uses PortAudio to handle the real work. */
  
  class RESONANT_API AudioLoop
  {
  public:
    AudioLoop();
    virtual ~AudioLoop();

    static bool init();
    static bool cleanup();
    
    bool startReadWrite(int samplerate, int channels);
    bool isRunning() { return m_isRunning; }

    bool stop();

    int outChannels() const;

  private:
    virtual void finished();

    virtual int callback(const void *in, void *out,
        unsigned long framesPerBuffer
//        , const PaStreamCallbackTimeInfo* time,
//        PaStreamCallbackFlags status
        ) = 0;

    bool       m_isRunning;
    bool       m_continue;

    class AudioLoopInternal;

    AudioLoopInternal * m_d;
 };

}


#endif

