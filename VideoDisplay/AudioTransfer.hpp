/* COPYRIGHT
 *
 * This file is part of VideoDisplay.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "VideoDisplay.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VIDEODISPLAY_AUDIO_TRANSFER_HPP
#define VIDEODISPLAY_AUDIO_TRANSFER_HPP

#include <Radiant/IODefs.hpp>
#include <Radiant/TimeStamp.hpp>

#include <Resonant/Module.hpp>

namespace VideoDisplay {

  class VideoIn;

  class AudioTransfer : public Resonant::Module
  {
  public:
    AudioTransfer(Resonant::Application *, VideoIn * video);
    virtual ~AudioTransfer();

    virtual bool prepare(int & channelsIn, int & channelsOut);
    virtual void process(float ** in, float ** out, int n);
    virtual bool stop();

    bool started() const { return m_started; }
    bool stopped() const { return m_stopped; }

    Radiant::TimeStamp audioTime();

  protected:
    VideoIn * m_video;
    int       m_channels;
    bool      m_started;
    bool      m_stopped;
    Radiant::AudioSampleFormat m_sampleFmt;
    long      m_frames;
  };

}

#endif

