/* COPYRIGHT
 *
 * This file is part of Watcher.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "FireView.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifdef WIN32
#include <WinPort.h>
#endif

#include <Radiant/TimeStamp.hpp>
#include <Radiant/Video1394.hpp>

#include <Nimble/Histogram.hpp>
#include <Nimble/Math.hpp>

#include <Magick++.h> 

#include <assert.h>

/* Capture and save images every sometimes, useful for video
   surveillance. */

int main(int, char **)
{
  Radiant::Video1394 video;

  video.open(0, 0, 0, Radiant::IMAGE_UNKNOWN, 640, 480, Radiant::FPS_10);

  Radiant::TimeStamp latest(Radiant::TimeStamp::getTime());

  video.start();

  float shutter = 0.2f;
  float gain = 0.5f;
  float step = 0.01;

  video.setGain(gain);
  video.setShutter(shutter);

  while(true) {
    const Radiant::VideoImage * img = video.captureImage();

    assert(img != 0);

    Radiant::TimeStamp now(Radiant::TimeStamp::getTime());
    
    if(Radiant::TimeStamp(now - latest).secondsD() > 10.0f) {
      // Save image:
      Magick::Image saver;
      
      const uint8_t * ptr = img->m_planes[0].m_data;
      int pn = img->pixels();

      Magick::Blob blob(ptr , pn);
      saver.size( "640x480");
      saver.magick( "GRAY" );
      saver.read( blob);

      time_t t = time(0);
      struct tm * lt = localtime( & t);
      char buf[64], buf2[64];
      sprintf(buf, "frames/watcher-frame-%.2d-%.2d-%.2dT%.2d-%.2d-%.2d.jpg",
	      lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
	      lt->tm_hour, lt->tm_min, lt->tm_sec);
      saver.write(buf);
      sprintf(buf2, "./gotimage %s &", buf);
      system(buf2);

      // Adjust camera:
      Nimble::Histogram<int, 256> hist;
      hist.clear();

      for(const uint8_t * sentinel = ptr + pn; ptr < sentinel; ptr++)
	hist.putSafe(*ptr);

      int bright = hist.getHighValueRelative(0.3);

      if(bright < 120) {
        shutter += step;
        if(bright < 50)
          shutter += step;
      }
      if(bright > 150) {
	shutter -= step;
        if(bright > 220)
          shutter -= step;
      }

      shutter = Nimble::Math::Clamp(shutter, 0.0f, 1.0f);

      if(shutter < 0.1) {
	gain -= step;
      }
      else if(shutter > 0.8) {
	gain += step;
      }

      gain = Nimble::Math::Clamp(gain, 0.0f, 1.0f);

      if(shutter < 0.5 && gain > 0.3) {
	gain -= step;
      }
      
      printf("pn & b & s & g = %d %d %f %f", pn, bright, shutter, gain);
      fflush(0);

      video.setShutter(shutter);
      video.setGain(gain);

      latest = now;
    }

    video.doneImage();

  }

  return 0;
}
