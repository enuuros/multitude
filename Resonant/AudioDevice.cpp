/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Resonant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "AudioDevice.hpp"

#include <Radiant/Trace.hpp>

#define RES_AUDIO_MAX_BUF_SIZE 4096

namespace Resonant {
  
  using namespace Radiant;

  AudioDevice::AudioDevice()
    : m_handle(0),
      m_samplerate(0),
      m_channels(0),
      m_interleaved(true),
      m_alsaSampleFormat(SND_PCM_FORMAT_S16_LE)
  {}
  
  AudioDevice::~AudioDevice()
  {
    close();
  }

  bool AudioDevice::open(const char * device, 
			 int samplerate, int channels,
			 bool input)
  {
    const char * fname = "AudioDevice::open";

    if(m_handle) {
      trace("%s # already open, reopening %s\n", fname, device);
      close();
    }
    
    if(!device)
      device = "hw:0,0";
    // device = "hw:0,0";

    trace("%s # %d Hz, %d channels, %s", fname, samplerate, channels,
	  input ? "capture" : "playback");

    m_input = input;

    int err;
    static snd_output_t *output = 0;

    snd_pcm_hw_params_t * hw_params = 0;
    snd_pcm_sw_params_t * sw_params = 0;

    unsigned int buftime = 300000;  // buffer, microseconds
    unsigned int pertime = 100000;  // period, microseconds

    snd_output_stdio_attach(&output, stdout, 0);

    if((err = snd_pcm_open
	(& m_handle, device,
	 input ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      error("%s # snd_pcm_open failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }
    
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
      error("%s # snd_pcm_hw_params_malloc failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }

    if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
      error("%s # snd_pcm_sw_params_malloc failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }

    if ((err = snd_pcm_hw_params_any (m_handle, hw_params)) < 0) {
      error("%s # snd_pcm_hw_params_any failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }

    if ((err = snd_pcm_hw_params_set_access
	 (m_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
      error("%s # snd_pcm_hw_params_set_access failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }

    m_interleaved = true;

    if((err = snd_pcm_hw_params_set_channels
	(m_handle, hw_params, channels)) < 0) {
      error("%s # snd_pcm_hw_params_set_channels failed %s (%d, %s)\n",
	    fname, device, channels, snd_strerror (err));
      return false;
    }

    m_channels = channels;
   
    snd_pcm_format_t format_list[] = {
      SND_PCM_FORMAT_S32_LE,
      SND_PCM_FORMAT_S16_LE,
      (snd_pcm_format_t) 0
    };
    
    for(int i = 0; (m_alsaSampleFormat = format_list[i]) != 0; i++) {
      if((err = snd_pcm_hw_params_set_format
	  (m_handle, hw_params, m_alsaSampleFormat)) == 0) {
	break;
      }
    }

    if(m_alsaSampleFormat == 0) {
      error("%s # snd_pcm_hw_params_set_format failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }
    
    trace("%s # got sample format %s",
	  fname, snd_pcm_format_name(m_alsaSampleFormat));

    unsigned int sr = samplerate;

    if(snd_pcm_hw_params_set_rate(m_handle, hw_params, sr, 0) != 0) {
      trace("%s # Cannot set exact rate, getting a nearby rate", fname);
      if((err = snd_pcm_hw_params_set_rate_near
	   (m_handle, hw_params, & sr, 0)) < 0) {
	error("%s # snd_pcm_hw_params_set_rate_near failed %s (%s)\n",
	      fname, device, snd_strerror (err));
	return false;
      }
    }

    uint num, den;
    snd_pcm_hw_params_get_rate_numden(hw_params, & num, & den);

    trace("%s # got sample rate %u %.0f", fname, sr, num / (float) den);

    m_bytesPerFrame = (m_alsaSampleFormat == SND_PCM_FORMAT_S16_LE) ?
      2 * channels : 4 * channels;
    int dir = 0;
    if((err = snd_pcm_hw_params_set_buffer_time_near
	(m_handle, hw_params, &buftime, &dir)) < 0) {
      error("%s # snd_pcm_hw_params_set_buffer_time_near failed %s (%d, %s)\n",
	    fname, device, (int) buftime, snd_strerror (err));
      // return false;
    }

    trace("%s # Got buftime %d", fname, (int) buftime);

    if((err = snd_pcm_hw_params_set_period_time_near
	(m_handle, hw_params, &pertime, &dir)) < 0) {
      error("%s # snd_pcm_hw_params_set_period_time_near failed %s (%d, %s)\n",
	    fname, device, (int) pertime, snd_strerror (err));
      // return false;
    }

    if ((err = snd_pcm_hw_params (m_handle, hw_params)) < 0) {
      error("%s # snd_pcm_hw_params failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }

    snd_pcm_uframes_t bufsize = 12345;
    snd_pcm_uframes_t periodsize = 12345;

    if((err = snd_pcm_hw_params_get_buffer_size(hw_params, & bufsize))) {
       error("%s # snd_pcm_hw_params_get_buffer_size "
	     "%s (%d, %s)\n",
	     fname, device, (int) bufsize, snd_strerror (err));
       // return false;
       bufsize = 2048;
    }

    if((err = snd_pcm_hw_params_get_period_size(hw_params, 
						& periodsize, & dir))) {
       error("%s # snd_pcm_hw_params_get_period_size "
	     "%s (%d, %s)\n",
	     fname, device, (int) periodsize, snd_strerror (err));
       // return false;
       periodsize = 512;
    }
    

    /* Adjust software parameters. */

    if((err = snd_pcm_sw_params_current(m_handle, sw_params))) {
      error("%s # snd_pcm_sw_params_current %s\n", 
	    fname, snd_strerror(err));
      return false;
    }

    if((err = snd_pcm_sw_params_set_start_threshold
	(m_handle, sw_params, (bufsize / periodsize) * periodsize))) {
      error("%s # snd_pcm_sw_params_set_start_threshold %s\n",
	    fname, snd_strerror(err));
      return false;
    }

    /* Do transfer when at least periodsize samples can be processed */
    if((err = snd_pcm_sw_params_set_avail_min(m_handle,
					      sw_params, periodsize))) {
      error("%s # snd_pcm_sw_params_set_avail_min %s\n",
	    fname, snd_strerror(err));
      return false;
    }

    if((err = snd_pcm_sw_params_set_xfer_align(m_handle, sw_params, 1))) {
      error("%s # snd_pcm_sw_params_set_xfer_align %s\n",
	    fname, snd_strerror(err));
      return false;
    }

    if((err = snd_pcm_sw_params(m_handle, sw_params))) {
      error("%s # snd_pcm_sw_params %s\n", fname, snd_strerror(err));
      return false;
    }

    snd_pcm_hw_params_free(hw_params);
    snd_pcm_sw_params_free(sw_params);
    
    if ((err = snd_pcm_prepare (m_handle)) < 0) {
      error("%s # snd_pcm_prepare failed %s (%s)\n",
	    fname, device, snd_strerror (err));
      return false;
    }
    
    snd_pcm_dump(m_handle, output);

    trace("%s # audio device opened (%s)", fname, device);
    
    m_device = device;
    m_samplerate = samplerate;
    
    m_buf.resize(RES_AUDIO_MAX_BUF_SIZE * m_bytesPerFrame);
    //RES_AUDIO_MAX_BUF_SIZE * m_bytesPerFrame);


    return true;
  }

  bool AudioDevice::start()
  {
    int err = snd_pcm_start(m_handle);

    if(err != 0) {
      error("AudioDevice::start # snd_pcm_start failed %s (%s)\n",
	    m_device.c_str(), snd_strerror (err));
      
      return false;
    }

    return true;
  }

  int AudioDevice::readInterleaved(float * samples, int frames)
  {
    int err, r = frames;

    while(frames) {
      int toread = (frames <= RES_AUDIO_MAX_BUF_SIZE) ?
	frames : RES_AUDIO_MAX_BUF_SIZE;
      
      if ((err = snd_pcm_readi (m_handle, & m_buf[0], toread)) != toread) {
	error("AudioDevice::readInterleaved # snd_pcm_readi failed %s",
	      snd_strerror(err));
	snd_pcm_prepare(m_handle);
	return -1;
      }
      
      if(m_alsaSampleFormat == SND_PCM_FORMAT_S32_LE)
	convert(samples, (const int32_t *) & m_buf[0], toread * m_channels);
      if(m_alsaSampleFormat == SND_PCM_FORMAT_S16_LE)
	convert(samples, (const int16_t *) & m_buf[0], toread * m_channels);
      
      samples += toread * m_channels;
      frames -= toread;
    }

    return r;
  }
  
  int AudioDevice::writeInterleaved(const float * samples, int frames)
  {
    int err, r = frames;

    while(frames) {
      int towrite = (frames <= RES_AUDIO_MAX_BUF_SIZE) ?
	frames : RES_AUDIO_MAX_BUF_SIZE;
      
      bzero( & m_buf[0], m_buf.size());

      if(m_alsaSampleFormat == SND_PCM_FORMAT_S32_LE)
	convert((int32_t *) & m_buf[0], samples, towrite * m_channels);
      if(m_alsaSampleFormat == SND_PCM_FORMAT_S16_LE)
	convert((int16_t *) & m_buf[0], samples, towrite * m_channels);
       
      // towrite *= 2;
      if ((err = snd_pcm_writei (m_handle, & m_buf[0], towrite)) != towrite) {
	error("AudioDevice::writeInterleaved # snd_pcm_writei failed %s",
	      snd_strerror(err));
	snd_pcm_prepare(m_handle);
	return -1;
      }
      
      samples += towrite * m_channels;
      frames -= towrite;
    }

    return r;
  }
  
  bool AudioDevice::close()
  {
    bool ok = m_handle != 0;

    if(m_handle) {
      snd_pcm_close(m_handle);
      m_handle = 0;
    }
    m_channels = 0;

    return ok;
  }

  void AudioDevice::convert(float * to, const int16_t * from, int n)
  {
    const float scale = 1.0f / 32768.0f;

    for(float * sentinel = to + n; to < sentinel; ) {
      *to++ = *from++ * scale;
    }
  }

  void AudioDevice::convert(float * to, const int32_t * from, int n)
  {
    const float scale = 1.0f / 2147483648.0f;

    for(float * sentinel = to + n; to < sentinel; ) {
      *to++ = *from++ * scale;
    }
  }
 
  template <class T, class S> inline T fast_round(S v)
  {
    return (v > 0) ? (T) (v + (S) 0.5) : (T) (v - (S) 0.5);
  }

  void AudioDevice::convert(int16_t * to, const float * from, int n)
  {
    const float scale = 32768.0f;

    for(int16_t * sentinel = to + n; to < sentinel; ) {
      *to++ = fast_round<int16_t, float>(*from++ * scale);
    }
  }

  void AudioDevice::convert(int32_t * to, const float * from, int n)
  {
    const float scale = 2147483648.0f;

    for(int32_t * sentinel = to + n; to < sentinel; ) {
      *to++ = fast_round<int32_t, float>(*from++ * scale);
    }
  }


}
