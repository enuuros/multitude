/* COPYRIGHT
 *
 * This file is part of Resonant.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Resonant.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef RESONANT_AUDIO_DEVICE_HPP
#define RESONANT_AUDIO_DEVICE_HPP

#include <alsa/asoundlib.h>

#include <string>
#include <vector>

namespace Resonant {
  
  class AudioDevice
  {
  public:
    AudioDevice();
    ~AudioDevice();

    bool open(const char * device, 
	      int samplerate, int channels, bool input);

    bool openRead(const char * device, 
		  int samplerate, int channels)
    { return open(device, samplerate, channels, true); }

    bool openWrite(const char * device, 
		   int samplerate, int channels)
    { return open(device, samplerate, channels, false); }

    bool start();

    int readInterleaved(float * samples, int frames);
    int writeInterleaved(const float * samples, int frames);

    int channels() const { return m_channels; }

    // bool stop();

    bool close();

    static void convert(float * to, const int16_t * from, int n);
    static void convert(float * to, const int32_t * from, int n);

    static void convert(int16_t * to, const float * from, int n);
    static void convert(int32_t * to, const float * from, int n);

  private:
    snd_pcm_t           * m_handle;
    std::vector<int8_t>   m_buf;
    int                   m_samplerate;
    int                   m_channels;
    std::string           m_device;
    bool                  m_interleaved;
    snd_pcm_format_t      m_alsaSampleFormat;
    int                   m_bytesPerFrame;
    bool                  m_input;
  };

}

#endif
