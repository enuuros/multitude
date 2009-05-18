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


#ifndef VIDEODISPLAY_VIDEO_IN_HPP
#define VIDEODISPLAY_VIDEO_IN_HPP

#include <Nimble/Vector2.hpp>

#include <Radiant/Condition.hpp>
#include <Radiant/VideoImage.hpp>
#include <Radiant/IODefs.hpp>
#include <Radiant/Mutex.hpp>
#include <Radiant/RefPtr.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/TimeStamp.hpp>

#include <VideoDisplay/Export.hpp>

#include <sys/types.h>

#include <vector>

namespace VideoDisplay {

  /// Base class for video file inputs, for the VideoDisplay framework
  class VideoIn : public Radiant::Thread
  {
  public:

    enum {
      MAX_AUDIO_CHANS = 5,
      MAX_AUDIO_SAMPLES_IN_FRAME = MAX_AUDIO_CHANS * 28000
    };
    
    enum FrameType {
      FRAME_INVALID,
      FRAME_IGNORE,
      FRAME_STREAM,
      FRAME_LAST,
      FRAME_SNAPSHOT
    };

    /* Request from the host. */ 
    enum Request {
      NO_REQUEST,
      START,
      SEEK,
      STOP
    };

    class VIDEODISPLAY_API Frame
    {
    public:
      Frame();
      ~Frame();

      void copyAudio(const void * audio, int channels, int frames,
                     Radiant::AudioSampleFormat format,
                     Radiant::TimeStamp ts)
      {
        int n = frames * channels;
        
        if(format == Radiant::ASF_INT16) {
          const int16_t * au16 = (const int16_t *) audio;
          
          for(int i = 0; i < n; i++)
            m_audio[i] = au16[i] * (1.0f / (1 << 16));
        }

        m_audioFrames = frames;
        m_audioTS = ts;
      }

      Radiant::VideoImage m_image;
      Radiant::TimeStamp m_time;
      Radiant::TimeStamp m_absolute;
      Radiant::TimeStamp m_audioTS;
      float     m_audio[MAX_AUDIO_SAMPLES_IN_FRAME];
      int       m_audioFrames;
      FrameType m_type;
    };

    class VIDEODISPLAY_API VideoInfo
    {
    public:
      VideoInfo()
	: m_videoFrameSize(0, 0),
	  m_videoDurationSecs(0),
	  m_fps(-1)
      {}

      Nimble::Vector2i m_videoFrameSize;
      double           m_videoDurationSecs;
      double           m_fps;
    };

    VIDEODISPLAY_API VideoIn();
    VIDEODISPLAY_API virtual ~VideoIn();

    
    VIDEODISPLAY_API Frame * getFrame(int i, bool updateCounter);

    VIDEODISPLAY_API virtual bool init(const char * filename, Radiant::TimeStamp pos);
    VIDEODISPLAY_API virtual bool play(Radiant::TimeStamp pos = -1);
    VIDEODISPLAY_API virtual void stop();
    VIDEODISPLAY_API virtual bool seek(Radiant::TimeStamp pos);

    VIDEODISPLAY_API virtual void getAudioParameters(int * channels, 
				    int * sample_rate,
				    Radiant::AudioSampleFormat * format) = 0;

    VIDEODISPLAY_API virtual float fps() = 0;

    int latestFrame() const { return m_decodedFrames - 1; }
    VIDEODISPLAY_API bool atEnd();
    bool isFrameAvailable(int frame) const
    { return(int) m_decodedFrames > frame && frame >= 0;}

    /// Finds the closest frame to the given time
    int selectFrame(int starfrom, Radiant::TimeStamp time) const;
    uint decodedFrames() const { return m_decodedFrames; }
    uint frameRingBufferSize() const { return m_frames.size(); }

    virtual double durationSeconds() = 0;

    uint finalFrames()   const { return m_finalFrames; }

    const char * name() { return m_name.c_str(); }

    VIDEODISPLAY_API static void setDebug(int level);
    VIDEODISPLAY_API static void toggleDebug();

    const VideoInfo & vdebug() const { return m_info; }

  protected:

    VIDEODISPLAY_API virtual void childLoop () ;

    VIDEODISPLAY_API virtual bool open(const char * filename, Radiant::TimeStamp pos) = 0;

    VIDEODISPLAY_API bool playing() { return m_playing; }
    // Get snapshot of the video in the given position
    VIDEODISPLAY_API virtual void videoGetSnapshot(Radiant::TimeStamp pos) = 0;
    // Start playing the video in the given position
    VIDEODISPLAY_API virtual void videoPlay(Radiant::TimeStamp pos) = 0;
    // Get the next next frame
    VIDEODISPLAY_API virtual void videoGetNextFrame() = 0;
    // Stop the video
    VIDEODISPLAY_API virtual void videoStop() = 0;

    /** An implmentation should use the methods below: */
    VIDEODISPLAY_API void allocateFrames(uint frameCount, uint width, uint height, 
			Radiant::ImageFormat fmt);

    VIDEODISPLAY_API void deallocateFrames();

    VIDEODISPLAY_API Frame * putFrame(const Radiant::VideoImage *,
		     FrameType type,
		     Radiant::TimeStamp show, 
		     Radiant::TimeStamp absolute);

    VIDEODISPLAY_API void ignorePreviousFrames();

    std::vector<Radiant::RefPtr<Frame> > m_frames;

    VideoInfo m_info;

    volatile uint m_decodedFrames;
    volatile uint m_consumedFrames;
    volatile uint m_consumedAuFrames;
    volatile uint m_finalFrames;

    volatile bool m_breakBack;
    volatile bool m_playing;

    int m_channels;
    int m_sample_rate;
    Radiant::AudioSampleFormat m_auformat;

    uint m_auBufferSize;
    uint m_auFrameBytes;

    volatile bool m_continue;

    Radiant::Condition m_vcond;
    Radiant::MutexAuto m_vmutex;

    Radiant::Condition m_acond;
    Radiant::MutexAuto m_amutex;


    float          m_fps;
    bool           m_done;
    bool           m_ending;

    std::string    m_name;

    static int     m_debug;

    volatile Request   m_request;
    Radiant::TimeStamp m_requestTime;
    Radiant::MutexAuto m_requestMutex;

    Radiant::TimeStamp m_frameTime;

  private:
    /// Disabled
    VideoIn(const VideoIn & ) : Radiant::Thread() {}
  };

}

#endif
