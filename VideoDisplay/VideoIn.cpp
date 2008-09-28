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

#include <Radiant/Trace.hpp>

#include "VideoIn.hpp"

#include <assert.h>

namespace VideoDisplay {

  using Radiant::error;
  using Radiant::trace;

  int VideoIn::m_debug = 0;

  VideoIn::VideoIn()
    : m_decodedFrames(0),
      m_consumedFrames(0),
      m_decodedAuFrames(0),
      m_consumedAuFrames(0),
      m_auBufferSize(0),
      m_auFrameBytes(0),
      m_continue(true),
      m_seekTarget(-1),
      m_seekNot(0),
      m_fps(0.0),
      m_done(false)
  {}

  VideoIn::~VideoIn()
  {
    deallocateFrames();
  }

  /** Gets the next frame from the video stream to be shown on the
      screen. */

  VideoIn::Frame * VideoIn::nextFrame()
  {
    /* qDebug("VideoIn::nextFrame # dec = %u cons = %u", 
       m_decodedFrames, m_consumedFrames); */

    m_vmutex.lock();
    while(m_decodedFrames <= m_consumedFrames && m_continue)
      m_vcond.wait(m_vmutex, 500);
    // m_vcond.wait(1000);
    m_vmutex.unlock();

    if(!m_continue && m_decodedFrames <= m_consumedFrames)
      return 0;

    Frame * im = & m_frames[m_consumedFrames % m_frames.size()];

    m_consumedFrames++;

    // Signal that one frame was consumed:
    m_vcond.wakeAll();

    return im;
  }

  bool VideoIn::startDecoding(const char * filename, Radiant::TimeStamp pos)
  {
    assert(!isRunning());

    m_finalFrames   = (uint) -1;
    m_finalAuFrames = (uint) -1;

    m_continue = false;

    bool ok = open(filename, pos);

    // m_fps = fps();

    if(!ok) {
      error("VideoIn::startDecoding # Could not open file \"%s\"", filename);
      return false;
    }

    m_continue = true;

    run();

    return true;
  }

  void VideoIn::stopDecoding()
  {
    if(!m_continue && !isRunning())
      return;

    m_continue = false;

    m_acond.wakeOne();
    m_vcond.wakeOne();

    bool ok = waitEnd();

    if(!ok) {
      error("VideoIn::stopDecoding # Failed to stop");
      kill();
      waitEnd();
    }
  }

  const void * VideoIn::getAudio(int * frames, bool block)
  {
    if((uint) (m_consumedAuFrames + *frames) >= m_finalAuFrames)
      *frames = m_finalAuFrames - m_consumedAuFrames;

    if(! *frames) {
      return 0;
    }

    uint index = m_consumedAuFrames % m_auBufferSize;

    uint untilWrap = m_auBufferSize - index;

    uint n = * frames;

    if(n >= untilWrap)
      n = untilWrap;

    /* qDebug("VideoIn::getAudio # n = %u dec = %u cons = %u cont = %d", 
       n, m_decodedAuFrames, m_consumedAuFrames, (int) m_continue); */
    m_amutex.lock();

    if(block) {
      while(m_decodedAuFrames <= (m_consumedAuFrames + n) && m_continue)
	// m_acond.wait(1000);
	m_acond.wait(m_amutex, 500);
    }
    else {
      if(m_decodedAuFrames <= (m_consumedAuFrames + n) && !m_continue) {
	* frames = 0;
	return 0;
      }
    }
    m_amutex.unlock();
    
    * frames = n;
    m_consumedAuFrames += n;

    // assert(m_consumedAuFrames <= m_decodedAuFrames);
    if(m_consumedAuFrames > m_decodedAuFrames) {
      trace("VideoIn::getAudio # Dangerous times, I guess you were seeking");
      m_consumedAuFrames -= n;
      * frames = 0;
      return 0;
    }

    m_acond.wakeAll();

    return & m_audio[index * m_auFrameBytes];
  }

  bool VideoIn::atEnd()
  {
    if((m_consumedAuFrames >= m_finalAuFrames) ||
       (m_consumedFrames >= m_finalFrames))
      return true;

    return false;
  }

  void VideoIn::setDebug(int level)
  {
    m_debug = level;
  }


  void VideoIn::toggleDebug()
  {
    m_debug = !m_debug;
  }

  /** Before we get here the video stream should be opened successfully
      and the ring buffers should be allocated. */

  void VideoIn::childLoop()
  {
    while(m_continue) {
      getNextFrame();
    }
  }

  void VideoIn::allocateFrames(uint frameCount, uint width, uint height, 
			       Radiant::ImageFormat fmt)
  {
    deallocateFrames();

    m_frames.resize(frameCount);

    for(uint i = 0; i < frameCount; i++) {
      m_frames[i].m_image.allocateMemory(fmt, width, height);
      m_frames[i].m_time = 0;
    }
  }


  void VideoIn::deallocateFrames()
  {
    for(uint i = 0; i < m_frames.size(); i++) {
      m_frames[i].m_image.freeMemory();
      m_frames[i].m_time = 0;
    }
  }

  void VideoIn::putFrame(const Radiant::VideoImage * im,  Radiant::TimeStamp show, 
                         Radiant::TimeStamp absolute)
  {

    if(m_debug)
      trace("VideoIn::putFrame # %u %u", m_decodedFrames, m_consumedFrames);

    assert(m_frames.size() != 0);

    m_vmutex.lock();
    while((m_decodedFrames + 1)
	  >= (m_consumedFrames + m_frames.size()) && m_continue)
      // m_vcond.wait(1000);
      m_vcond.wait(m_vmutex, 500);
    m_vmutex.unlock();

    if(!m_continue)
      return;

    Frame & f = m_frames[m_decodedFrames % m_frames.size()];

    f.m_time = show;
    f.m_absolute = absolute;

    bool ok = f.m_image.copyData(*im);

    if(!ok)
      error("VideoIn::putFrame # Radiant::Image::copyData failed");

    m_decodedFrames++;

    // Signal that one frame was produced:
    m_vcond.wakeAll();

    // qDebug("VideoIn::putFrame # EXIT");
  }

  void VideoIn::allocateAudioBuffer(uint frameCount, 
				    uint channels, 
				    Radiant::AudioSampleFormat fmt)
  {
    uint sampleBytes = Radiant::sampleWidth(fmt);
    m_auBufferSize = frameCount;
    m_auFrameBytes = sampleBytes * channels;

    m_audio.resize(frameCount * m_auFrameBytes);
  }

  void VideoIn::putAudio(const void * audio_data, int audio_frames)
  {
    if(m_debug)
      trace("VideoIn::putAudio # n = %d dec = %u cons = %u cont = %d", 
	    audio_frames, m_decodedAuFrames, m_consumedAuFrames,
	    (int) m_continue);

    if(!audio_frames)
      return;
  
    assert(audio_frames < (int) m_auBufferSize);

    m_amutex.lock();
    m_breakBack = false;
    while(((m_decodedAuFrames + audio_frames) >= 
	   (m_consumedAuFrames + m_auBufferSize)) && 
	  m_continue && !m_breakBack)
      // m_acond.wait(1000);
      m_acond.wait(m_amutex, 500);
    m_amutex.unlock();

    if(!m_continue)
      return;

    uint pos = m_decodedAuFrames % m_auBufferSize;
    uint avail = m_auBufferSize - pos;
  
    if((int) avail < audio_frames) {
      uint bytes = avail * m_auFrameBytes;

      memcpy( & m_audio[(m_decodedAuFrames % m_auBufferSize) * m_auFrameBytes], 
	      audio_data, bytes);

      audio_frames -= avail;
      m_decodedAuFrames += avail;

      const char * tmp = (const char *) audio_data;
      tmp += bytes;
      audio_data = tmp;
    }

    uint bytes = audio_frames * m_auFrameBytes;

    memcpy( & m_audio[(m_decodedAuFrames % m_auBufferSize) * m_auFrameBytes], 
	    audio_data, bytes);

    m_decodedAuFrames += audio_frames;
  
    m_acond.wakeAll();
  }

}
