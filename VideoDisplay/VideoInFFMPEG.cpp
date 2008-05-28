/* COPYRIGHT
 *
 * This file is part of Valueable.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Valueable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "VideoInFFMPEG.hpp"

#include <Radiant/Trace.hpp>

#include <vector>

namespace VideoDisplay {

  using namespace Radiant;

  VideoInFFMPEG::VideoInFFMPEG()
    : m_needSync(0),
      m_gotSync(0)
  {}

  VideoInFFMPEG::~VideoInFFMPEG()
  {}

  void VideoInFFMPEG::getAudioParameters(int * channels, 
				     int * sample_rate,
				     AudioSampleFormat * format)
  {
    m_video.getAudioParameters(channels, sample_rate, format);
  }

  float VideoInFFMPEG::fps()
  {
    float fps = m_fps > 0.0f ? m_fps : m_video.fps();

    if(fps <= 1.0f)
      fps = 30.0f;

    while(fps > 100)
      fps = fps / 10.0f;

    return fps;
  }

  bool VideoInFFMPEG::open(const char * filename, Radiant::TimeStamp pos)
  {
    static const char * fname = "VideoInFFMPEG::open";

    if(!m_video.open(filename, WITH_VIDEO | WITH_AUDIO))
      return false;

    m_seekTarget = -1.0f;

    if(!m_video.hasVideoCodec()) {
      Radiant::error("%s # No video codec", fname);
      m_video.close();
      return false;
    }

    if(!m_video.hasAudioCodec()) {
      Radiant::error("%s # No audio codec", fname);
      m_video.close();
      return false;
    }

    if(pos != 0) {
      Radiant::trace("%s # Doing a seek", fname);
      if(!m_video.seekPosition(pos.secondsD()))
	m_video.seekPosition(0);
    }

    const VideoImage * img = m_video.captureImage();

    if(!img)
      return false;

    if(!img->m_width)
      return false;

    float fp = fps();

    trace("%s # %f fps", fname, fp);

    float latency = 1.5f;

    allocateFrames((uint) (fp * latency), 
		   img->m_width, img->m_height, img->m_format);

    int channels, sample_rate;
    AudioSampleFormat fmt;

    m_video.getAudioParameters( & channels, & sample_rate, & fmt);

    if(channels) {
      allocateAudioBuffer((uint) (sample_rate * latency), channels, fmt);
    }

    putFrame(img, m_video.frameTime(), m_video.frameTime());

    needResync();
    
    m_finalFrames   = (uint) -1;
    m_finalAuFrames = (uint) -1;

    return true;
  }

  void VideoInFFMPEG::close()
  {
    m_video.close();
  }

  void VideoInFFMPEG::getNextFrame()
  {
    double st = -1.0;

    bool seek = false;

    if(m_seekNot == 0) {
      m_seekMutex.lock();    
      st = m_seekTarget;
      m_seekTarget = -1.0f;
      m_seekMutex.unlock();
    }
    else
      m_seekNot--;
    
    if(st >= 0.0f) {
      trace("VideoInFFMPEG::getNextFrame # Doing a seek to %lf", st);
      m_video.seekPosition(st);
      seek = true;
      needResync();
      trace("VideoInFFMPEG::getNextFrame # Seek done");
      m_seekNot = 4;
    }

    const VideoImage * img = m_video.captureImage();

    if(!img) {
      endOfFile();
      return;
    }

    int aframes = 0;
    
    const void * audio = m_video.captureAudio( & aframes);

    while(seek && aframes == 0 && img) {
      // We must find a frame with some audio so we can sync to that.

      img = m_video.captureImage();
      audio = m_video.captureAudio( & aframes);      
    }
    
    if(!img) {
      endOfFile();
      return;
    }

    if(seek) {
      doSeek(img, audio, aframes);
      return;
    }

    TimeStamp show, vt = m_video.frameTime();

    /* Now comes a tricky part. The audio lacks timestamp and
       therefore we do not know how to sync audio and video if the
       stream has jumped to some random position. To overcome this we
       guess that new audio chunks always match frame they come with
       (to the extent possible). */
    
    if(aframes && (m_gotSync < m_needSync)) {
      doSync(aframes, vt);
    }
    else if(m_gotSync == 0) {
      vt = 0;
    }

    if(audio && aframes) {
      putAudio(audio, aframes);
    }
    
    putFrame(img, vt + m_syncOffset, vt);
  }

  double VideoInFFMPEG::durationSeconds()
  {
    return m_video.durationSeconds();
  }

  bool VideoInFFMPEG::seekTo(double seconds)
  {
    m_seekMutex.lock();
    m_seekTarget = seconds;
    m_seekMutex.unlock();

    m_breakBack = true;
    m_acond.wakeAll();

    return true;
  }

  void VideoInFFMPEG::doSeek(const Radiant::VideoImage * im,
                             const void * audio, int audioframes)
  {
    /* Here we trash part of our (excessive) buffer, so that seek
       happens with short latency. */
    TimeStamp ts;

    {
      Radiant::Guard gv( & m_vmutex);
      Radiant::Guard ga( & m_amutex);
      
      long i, sr = m_video.audioSampleRate();
      
      long interval = m_decodedAuFrames - m_consumedAuFrames;
      long maxinterval = sr / 10;
      
      if(interval > maxinterval) {
        interval = maxinterval;
      }
      
      long hitAFrame = m_consumedAuFrames + interval;
      
      ts = TimeStamp::createSecondsD(hitAFrame / (double) sr);
      
      long hitVFrame = m_consumedFrames + 1;
      
      for(i = hitVFrame; i < (long) m_decodedFrames; i++) {
        Frame & f = m_frames[i % m_frames.size()];
        if(f.m_time > ts)
          break;
        else
          hitVFrame = i;
      }

      trace("VideoInFFMPEG::doSeek # VF: %u -> %ld AF: %u -> %ld",
            m_decodedFrames, hitVFrame, m_decodedAuFrames, hitAFrame);
    
      m_decodedFrames = hitVFrame;
      m_decodedAuFrames = hitAFrame;
    }

    doSync(audioframes, m_video.frameTime());

    putFrame(im, ts, m_video.frameTime());
    putAudio(audio, audioframes);
  }

  void VideoInFFMPEG::needResync()
  {
    m_needSync = 20;
    m_gotSync = 0;
    m_syncAccu = 0;
  }

  void VideoInFFMPEG::doSync(int aframes, Radiant::TimeStamp vt)
  {
    uint frameSample = m_decodedAuFrames;
    double frameTime = frameSample / (double) m_video.audioSampleRate();
    Radiant::TimeStamp frameTS = Radiant::TimeStamp::createSecondsD(frameTime);
    Radiant::TimeStamp offset = frameTS - vt;
    m_syncAccu += offset;
    m_gotSync++;
    m_syncOffset = m_syncAccu / m_gotSync;
    
    if(aframes >= 10000) {
      // Big burst of frames is good enough.
      m_needSync = 1;
    }
    
    trace("Sync offset is now %ld (%.2lf)", (long) m_syncOffset, 
          m_syncOffset.secondsD());    
  }

  void VideoInFFMPEG::endOfFile()
  {
    m_finalFrames = m_decodedFrames;
    m_finalAuFrames = m_decodedAuFrames;
    
    m_done = true;
    m_continue = false;

    
  }
}
