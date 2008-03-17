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


#ifndef VIDEODISPLAY_VIDEO_IN_FFMPEG_HPP
#define VIDEODISPLAY_VIDEO_IN_FFMPEG_HPP

#include <VideoDisplay/VideoIn.hpp>

#include <Radiant/VideoFFMPEG.hpp>

namespace VideoDisplay {

  class VideoInFFMPEG : public VideoIn
  {
  public:
    VideoInFFMPEG();
    virtual ~VideoInFFMPEG();

    virtual void getAudioParameters(int * channels, 
				    int * sample_rate,
				    Radiant::AudioSampleFormat * format);

    virtual float fps();

    virtual double durationSeconds();
    virtual bool seekTo(double seconds);
  
  protected:

    virtual bool open(const char * filename, Radiant::TimeStamp pos);
    virtual void close();
    virtual void getNextFrame();

    void doSeek(const Radiant::VideoImage * im, const void * audio, int audioframes);
    void needResync();
    void doSync(int aframes, Radiant::TimeStamp vt);
    void endOfFile();

    int m_needSync;
    int m_gotSync;
    Radiant::TimeStamp m_syncAccu;
    Radiant::TimeStamp m_syncOffset;

    RadiantIO::VideoInputFFMPEG m_video;
  };

}

#endif
