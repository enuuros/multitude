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

#include "VideoIn.hpp"

#include "AudioTransfer.hpp"

#include <Radiant/Sleep.hpp>
#include <Radiant/Trace.hpp>

#include <assert.h>
#include <string.h>

#include <map>

namespace VideoDisplay {

  using namespace Radiant;

  VideoIn::Frame::Frame()
    : m_audioFrames(0),
      m_type(FRAME_INVALID)
  {
    bzero(m_audio, sizeof(m_audio));
  }

  VideoIn::Frame::~Frame()
  {
    m_image.freeMemory();
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  static std::map<std::string, VideoIn::VideoInfo> __infos;

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  int VideoIn::m_debug = 1;

  VideoIn::VideoIn()
    : m_decodedFrames(0),
      m_consumedFrames(0),
      m_consumedAuFrames(0),
      m_playing(false),

      m_channels(0),
      m_sample_rate(44100),
      m_auformat(ASF_INT16),

      m_auBufferSize(0),
      m_auFrameBytes(0),
      m_continue(true),
      m_fps(0.0),
      m_done(false),
      m_request(NO_REQUEST),
      m_listener(0)
  {}

  VideoIn::~VideoIn()
  {
    if(m_listener)
      m_listener->forgetVideo();

    assert(!isRunning());

    /*
      The code below cannot be used to do anything. We are in the
      destructor, and virtual tables are long gone :-|

    if(isRunning()) {
      m_continue = false;
      m_vcond.wakeAll(m_vmutex);
      waitEnd();
    }
    */
  }

  /** Gets a frame from the video stream to be shown on the screen. */

  VideoIn::Frame * VideoIn::getFrame(int index, bool updateCount)
  {
    if(index < 0)
      return 0;

    /* Radiant::debug("VideoIn::nextFrame # dec = %u cons = %u", 
		  m_decodedFrames, m_consumedFrames);
    */
    assert((int) m_decodedFrames > index);

    if(!m_continue && m_decodedFrames <= m_consumedFrames)
      return 0;

    Frame * im = m_frames[index % m_frames.size()].ptr();

    if(updateCount) {
      m_consumedFrames = index;

      // Signal that one frame was consumed:
      m_vcond.wakeAll();
    }
    else {
      m_consumedAuFrames = index;

      // Signal that one frame was consumed:
      m_vcond.wakeAll();
    }

    return im;
  }

  bool VideoIn::init(const char * filename, Radiant::TimeStamp pos)
  {
    assert(!isRunning());

    m_finalFrames   = (uint) -1;

    m_continue = true;

    bool ok = open(filename, pos);

    if(!ok) {
      error("VideoIn::start # Could not open file \"%s\"", filename);
      m_continue = false;
      return false;
    }

    run();

    return true;
  }

  bool VideoIn::play(Radiant::TimeStamp pos)
  {
    debug("VideoIn::play");

    Guard g( & m_requestMutex);

    m_request = START;
    if(pos < 0)
      m_requestTime = m_frameTime;
    else
      m_requestTime = pos;

    return true;
  }

  void VideoIn::stop()
  {
    debug("VideoIn::stop");

    if(!m_continue && !isRunning())
      return;

    Guard g( & m_requestMutex);

    m_request = STOP;
    m_requestTime = 0;
  }

  bool VideoIn::seek(Radiant::TimeStamp pos)
  {
    debug("VideoIn::seek");

    Guard g( & m_requestMutex);

    m_request = SEEK;
    m_requestTime = pos;

    return true;
  }

  bool VideoIn::atEnd()
  {
    if((m_consumedFrames >= m_finalFrames))
      return true;

    return false;
  }

  int VideoIn::selectFrame(int, Radiant::TimeStamp time) const
  {
    int latest = latestFrame();

    int best = latest; // Nimble::Math::Min(latest, startfrom);
    int low = Nimble::Math::Min((int) m_consumedFrames, 
				(int) m_consumedAuFrames);
    // (int) (best - frameRingBufferSize() / 2));
    low = Nimble::Math::Max(low, 0);
    TimeStamp bestdiff = TimeStamp::createSecondsD(10000);

    double close = -1.0;
    
    for(int i = best; i >= low; i--) {
      const Frame * f = m_frames[i % m_frames.size()].ptr();

      if(!f)
	continue;

      if(f->m_type == FRAME_INVALID ||
         f->m_type == FRAME_IGNORE)
        continue;

      TimeStamp diff = Nimble::Math::Abs(f->m_absolute - time);
      if(diff < bestdiff) {
	best = i;
	bestdiff = diff;
	close = f->m_absolute.secondsD();
      }
      else
	break;
    }
    
    debug("VideoIn::selectFrame # %d (%d %d) (%d %d) %lf %lf",
	  best, low, latest, m_consumedFrames, m_consumedAuFrames,
	  close, time.secondsD());

    return best;
  }

  void VideoIn::setDebug(int level)
  {
    m_debug = level;
  }

  void VideoIn::toggleDebug()
  {
    m_debug = !m_debug;
  }

  void VideoIn::setAudioListener(AudioTransfer * listener)
  {
    debug("VideoIn::setAudioListener # from %p to %p", m_listener, listener);
    m_listener = listener;
  }

  /** Before we get here the video stream should be opened successfully
      and the ring buffers should be allocated. */

  void VideoIn::childLoop()
  {
    debug("VideoIn::childLoop # ENTRY");

    while(m_continue) {

      m_requestMutex.lock();
      Request r = m_request;
      TimeStamp rt = m_requestTime;
      m_request = NO_REQUEST;
      m_requestMutex.unlock();

      if(r != NO_REQUEST)
        debug("VideoIn::childLoop # REQ = %d p = %d",(int) r, (int) playing());

      if(r == START) {
	videoPlay(rt);
	m_playing = true;
      }
      else if(r == STOP) {
	videoStop();
	m_playing = false;
      }
      else if(r == SEEK) {
	if(playing())
	  videoPlay(rt);
	else 
	  videoGetSnapshot(rt);
      }
      else if(playing())
	videoGetNextFrame();

      Radiant::Sleep::sleepMs(5);
    }

    debug("VideoIn::childLoop # EXIT");
  }


  VideoIn::Frame * VideoIn::putFrame(const Radiant::VideoImage * im,
				     FrameType type,
				     Radiant::TimeStamp show, 
				     Radiant::TimeStamp absolute)
  {
    assert(m_frames.size() != 0);

    m_vmutex.lock();
    while(((m_decodedFrames + 4) >= (m_consumedFrames + m_frames.size()) ||
           (m_decodedFrames + 4) >= (m_consumedAuFrames + m_frames.size())) && 
          m_continue)
      // m_vcond.wait(1000);
      m_vcond.wait(m_vmutex, 500);
    m_vmutex.unlock();

    if(!m_continue)
      return 0;

    RefPtr<Frame> & rf = m_frames[m_decodedFrames % m_frames.size()];
    
    if(!rf.ptr())
      rf = new Frame;

    Frame & f = * rf.ptr();
    
    f.m_type = type;
    f.m_time = show;
    f.m_absolute = absolute;
    f.m_audioFrames = 0;
    f.m_audioTS = 0;

    if(type == FRAME_SNAPSHOT)
      m_consumedAuFrames = m_decodedFrames;
    
    if(!f.m_image.m_planes[0].m_data)
      f.m_image.allocateMemory(*im);

    bool ok = f.m_image.copyData(*im);

    if(!ok)
      error("VideoIn::putFrame # Radiant::Image::copyData failed");

    m_decodedFrames++;

    // Signal that one frame was produced:
    m_vcond.wakeAll();

    if(m_debug)
      debug("VideoIn::putFrame # %p %u %u %lf",
	    & f, m_decodedFrames, m_consumedFrames, absolute.secondsD());

    return & f;
    // qDebug("VideoIn::putFrame # EXIT");
  }

  void VideoIn::ignorePreviousFrames()
  {
    for(uint i = m_consumedFrames; (i + 1) < m_decodedFrames; i++)
      m_frames[i % m_frames.size()].ptr()->m_type = FRAME_IGNORE;
  }

}
