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

#include "VideoInFFMPEG.hpp"

#include <Radiant/Trace.hpp>

#include <vector>

namespace VideoDisplay {

  using namespace Radiant;

  VideoInFFMPEG::VideoInFFMPEG()
    : m_channels(0),
      m_sampleRate(44100),
      m_auformat(ASF_INT16)
  
  {
    m_audiobuf.resize(2 * 44100);
  }

  VideoInFFMPEG::~VideoInFFMPEG()
  {
    info("VideoInFFMPEG::~VideoInFFMPEG");
    if(isRunning()) {
      m_continue = false;
      m_vcond.wakeAll(m_vmutex);
      waitEnd();
    }
    info("VideoInFFMPEG::~VideoInFFMPEG # EXIT");
  }

  void VideoInFFMPEG::getAudioParameters(int * channels, 
					 int * sample_rate,
					 AudioSampleFormat * format)
  {
    * channels = m_channels;
    * sample_rate = m_sample_rate;
    * format = m_auformat;
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

    Radiant::info(fname);

    if(!m_video.open(filename, WITH_VIDEO | WITH_AUDIO))
      return false;

    m_video.getAudioParameters( & m_channels, & m_sample_rate, & m_auformat);

    m_name = filename;

    m_buffered = 0;

    if(!m_video.hasVideoCodec()) {
      Radiant::error("%s # No video codec", fname);
      m_video.close();
      return false;
    }

    if(!m_video.hasAudioCodec()) {
      Radiant::debug("%s # No audio codec", fname);
      /* m_video.close();
	 return false; */
    }
    pos = TimeStamp::createSecondsD(10.0);
    if(pos != 0) {
      debug("%s # Doing a seek", fname);
      if(!m_video.seekPosition(pos.secondsD()))
	m_video.seekPosition(0);
    }

    const VideoImage * img = m_video.captureImage();

    if(!img)
      return false;

    if(!img->m_width)
      return false;

    m_info.m_videoFrameSize.make(img->m_width, img->m_height);

    float fp = fps();

    m_duration = TimeStamp::createSecondsD(m_video.durationSeconds());

    debug("%s # %f fps", fname, fp);

    float latency = 1.5f;
    m_frames.resize(latency * fp);

    int channels, sample_rate;
    AudioSampleFormat fmt;

    m_video.getAudioParameters( & channels, & sample_rate, & fmt);

    putFrame(img, FRAME_SNAPSHOT, m_video.frameTime(), m_video.frameTime());

    m_video.close();

    info("%s # EXIT OK", fname);

    return true;
  }


  void VideoInFFMPEG::videoGetSnapshot(Radiant::TimeStamp pos)
  {
    info("VideoInFFMPEG::videoGetSnapshot");

    if(!m_video.open(m_name.c_str(), WITH_VIDEO | WITH_AUDIO)) {
      endOfFile();
      return;
    }

    if(pos)
      m_video.seekPosition(pos.secondsD());
    
    const VideoImage * img = m_video.captureImage();

    putFrame(img, FRAME_SNAPSHOT, 0, m_video.frameTime());

    m_video.close();
  }

  void VideoInFFMPEG::videoPlay(Radiant::TimeStamp pos)
  {
    info("VideoInFFMPEG::videoPlay # %lf", pos.secondsD());
    
    if(!m_video.open(m_name.c_str(), WITH_VIDEO | WITH_AUDIO)) {
      endOfFile();
      info("VideoInFFMPEG::videoPlay # Open failed for \"%s\"",
	   m_name.c_str());
      return;
    }

    m_channels = 0;
    m_sampleRate = 44100;
    m_auformat = ASF_INT16;

    m_video.getAudioParameters( & m_channels, & m_sampleRate, & m_auformat);

    if(pos > 0)
      m_video.seekPosition(pos.secondsD());

    const VideoImage * img = m_video.captureImage();

    if(!img) {
      info("VideoInFFMPEG::videoPlay # Image capture failed \"%s\"",
	   m_name.c_str());
      endOfFile();
      return;
    }

    m_frameTime = m_video.frameTime();

    putFrame(img, FRAME_STREAM, 0, m_video.frameTime());

    info("VideoInFFMPEG::videoPlay # EXIT OK");
  }

  void VideoInFFMPEG::videoGetNextFrame()
  {
    info("VideoInFFMPEG::videoGetNextFrame");

    const VideoImage * img = m_video.captureImage();

    if(!img) {
      endOfFile();
      return;
    }

    int aframes = 0;
    const void * audio = m_video.captureAudio( & aframes);

    TimeStamp vt = m_video.frameTime();
    
    m_frameDelta = m_frameTime.secsTo(vt);
    
    Frame * f = putFrame(img, FRAME_STREAM, vt + m_syncOffset, vt);
    
    if(aframes && f) {
      int n = aframes * m_channels;
      
      if(m_auformat == ASF_INT16) {
	const int16_t * au16 = (const int16_t *) audio;

	for(int i = 0; i < n; i++)
	  f->m_audio[i] = au16[i] * (1.0f / (1 << 16));
	
	f->m_audioFrames = aframes;
	f->m_audioTS = m_video.audioTime();
      }
    }
    else if(f) {
      f->m_audioFrames = 0;
      f->m_audioTS = 0;
    }
    m_frameTime = vt;
  }

  void VideoInFFMPEG::videoStop()
  {
    info("VideoInFFMPEG::videoStop");
    m_video.close();
  }

    /* Now comes a tricky part. The audio lacks timestamp and
       therefore we do not know how to sync audio and video if the
       stream has jumped to some random position. To overcome this we
       guess that new audio chunks always match frame they come with
       (to the extent possible). */
    

  double VideoInFFMPEG::durationSeconds()
  {
    return m_duration.secondsD();
  }

  void VideoInFFMPEG::endOfFile()
  {
    m_finalFrames = m_decodedFrames;
    m_finalAuFrames = m_decodedAuFrames;
    m_playing = false;
  }
}
