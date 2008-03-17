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


#ifndef VIDEODISPLAY_VIDEO_IN_HPP
#define VIDEODISPLAY_VIDEO_IN_HPP

#include <Radiant/Condition.hpp>
#include <Radiant/VideoImage.hpp>
#include <Radiant/IODefs.hpp>
#include <Radiant/Mutex.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/TimeStamp.hpp>

#include <sys/types.h>

#include <vector>

namespace VideoDisplay {

  class VideoIn : public Radiant::Thread
  {
  public:

    class Frame
    {
    public:
      Radiant::VideoImage m_image;
      Radiant::TimeStamp m_time;
      Radiant::TimeStamp m_absolute;
    };

    VideoIn();
    virtual ~VideoIn();

    Frame * nextFrame();

    virtual bool startDecoding(const char * filename, Radiant::TimeStamp pos);
    virtual void stopDecoding();

    virtual void getAudioParameters(int * channels, 
				    int * sample_rate,
				    Radiant::AudioSampleFormat * format) = 0;

    virtual float fps() = 0;

    const void * getAudio(int * frames, bool block);
    bool atEnd();

    uint decodedFrames() const { return m_decodedFrames; }
    uint frameRingBufferSize() const { return m_frames.size(); }

    bool done() { return m_done && 
	((m_consumedFrames + 4) >= m_decodedFrames);}

    virtual double durationSeconds() = 0;
    virtual bool seekTo(double seconds) = 0;

    uint finalFrames()   const { return m_finalFrames; }
    uint finalAuFrames() const { return m_finalAuFrames; }

    static void setDebug(int level);
    static void toggleDebug();

  protected:

    virtual void childLoop () ;

    virtual bool open(const char * filename, Radiant::TimeStamp pos) = 0;
    virtual void close() = 0;
    virtual void getNextFrame() = 0;

    /** An implmentation should use the methods below: */
    void allocateFrames(uint frameCount, uint width, uint height, 
			Radiant::ImageFormat fmt);

    void deallocateFrames();

    void putFrame(const Radiant::VideoImage *, Radiant::TimeStamp show, 
                  Radiant::TimeStamp absolute);

    void allocateAudioBuffer(uint frameCount, 
			     uint channels, 
			     Radiant::AudioSampleFormat fmt);

    void putAudio(const void * audio_data, int audio_frames);

    std::vector<Frame> m_frames;

    std::vector<char> m_audio;

    volatile uint m_decodedFrames;  
    volatile uint m_consumedFrames;
    volatile uint m_finalFrames;

    /// Audio frames:
    volatile uint m_decodedAuFrames;  
    volatile uint m_consumedAuFrames;
    volatile uint m_finalAuFrames;
    volatile bool m_breakBack;

    uint m_auBufferSize;
    uint m_auFrameBytes;

    volatile bool m_continue;

    Radiant::Condition m_vcond;
    Radiant::MutexAuto m_vmutex;

    Radiant::Condition m_acond;
    Radiant::MutexAuto m_amutex;

    Radiant::MutexAuto m_seekMutex;

    double         m_seekTarget;
    int            m_seekNot;

    float          m_fps;
    bool           m_done;

    static int     m_debug;

  private:
    /// Disabled
    VideoIn(const VideoIn & ) : Radiant::Thread() {}
  };

}

#endif
