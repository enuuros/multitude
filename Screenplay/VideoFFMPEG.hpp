/* COPYRIGHT
 *
 * This file is part of Screenplay.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Screenplay.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef SCREENPLAY_VIDEO_FFMPEG_HPP
#define SCREENPLAY_VIDEO_FFMPEG_HPP

#include <Radiant/TimeStamp.hpp>
#include <Radiant/VideoInput.hpp>

#include <ScreenPlay/Export.hpp>

#include <string>

extern "C" {

#include </*libavformat/*/avformat.h>
#include </*libavcodec/*/avcodec.h>
#include </*libavutil/*/avutil.h>

  struct AVCodec;
  struct AVCodecContext;
  struct AVFormatContext;
  struct AVFrame;
}

namespace Screenplay {

  class SCREENPLAY_API VideoInputFFMPEG : public Radiant::VideoInput
  {
  public:

    VideoInputFFMPEG();
    ~VideoInputFFMPEG();

    virtual const Radiant::VideoImage * captureImage();
    Radiant::TimeStamp frameTime() { return m_lastTS; }

    virtual const void * captureAudio(int * frameCount);
    virtual void getAudioParameters(int * channels, 
				    int * sample_rate,
				    Radiant::AudioSampleFormat * format);

    /// The width of the video stream images.
    virtual int width() const;
    /// The height of the video stream images.
    virtual int height() const;
    /// The frame rate of the video stream.
    virtual float fps() const;
    /// Native image format of the stream.
    virtual Radiant::ImageFormat imageFormat() const;  
    virtual unsigned int size() const;

    bool open(const char * filename,
	      int flags = Radiant::WITH_VIDEO);

    virtual bool start();
    virtual bool isStarted() const;
    virtual bool stop();
    virtual bool close();

    // void configure(const char * filename, const char * codecname = 0);

    long capturedAudio() { return m_capturedAudio; }
    long capturedVideo() { return m_capturedVideo; }

    bool seekPosition(double timeSeconds);
    double durationSeconds();

    bool hasAudioCodec() const { return m_acodec != 0; }
    bool hasVideoCodec() const { return m_vcodec != 0; }
  
    int audioSampleRate() const { return m_audioSampleRate; }

    /// Turn on/off the printing of debug messages
    static void setDebug(int debug);

  private:

    std::string      m_fileName;
    std::string      m_codecName;

    AVCodec        * m_acodec;
    int              m_aindex; // Audio index
    AVCodecContext * m_acontext;
    std::vector<int16_t> m_audioBuffer;
    int                  m_audioFrames;
    int                  m_audioChannels;
    int                  m_audioSampleRate;

    long m_capturedAudio;
    long m_capturedVideo;

    AVCodec        * m_vcodec;
    int              m_vindex; // Video index
    AVCodecContext * m_vcontext;
    AVFrame        * m_frame;

    AVFormatContext *m_ic;

    AVPacket         m_pkt;

    enum { BUFFER_SIZE = 4096 };

    Radiant::VideoImage m_image;

    int              m_flags;
    int64_t          m_lastPts;
    Radiant::TimeStamp  m_lastTS;
    double           m_lastSeek;

    int              m_sinceSeek;
    Radiant::TimeStamp  m_offsetTS;

    static int       m_debug;
  };

}

#endif
