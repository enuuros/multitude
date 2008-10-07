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


#ifndef VIDEODISPLAY_SHOW_GL_HPP
#define VIDEODISPLAY_SHOW_GL_HPP

#include <Luminous/GLSLProgramObject.hpp>
#include <Luminous/Texture.hpp>

#include <Nimble/Vector2.hpp>
#include <Nimble/Matrix3.hpp>

#include <Radiant/RefPtr.hpp>
#include <Radiant/TimeStamp.hpp>
#include <Radiant/VideoImage.hpp>

#include <Resonant/DSPNetwork.hpp>

#include <VideoDisplay/Export.hpp>
#include <VideoDisplay/SubTitles.hpp>
#include <VideoDisplay/VideoIn.hpp>

#include <GL/glew.h>

namespace Resonant {
  class DSPNetwork;
}

namespace Dyslexic {
  class GPUFont;
}

namespace VideoDisplay {

  using Nimble::Vector2;
  using Nimble::Vector2i;
  using namespace Radiant;

  class AudioTransfer;

  class ShowGL
  {
  public:

    class YUVProgram : public Luminous::GLSLProgramObject
    {
      public:
        YUVProgram(Luminous::GLResources * resources);
        virtual ~YUVProgram();

        bool init();
        virtual void bind();
        virtual void unbind();
        virtual bool link();
        virtual void clear();


      private:

        enum {
          PARAM_YTEX,
          PARAM_UTEX,
          PARAM_VTEX,
          PARAM_MATRIX,
          PARAM_SIZEOF
        };

        int m_uniforms[PARAM_SIZEOF];
    };

    class MyTextures : public Luminous::GLResource
    {
    public:
      MyTextures(Luminous::GLResources * resources);

      virtual void bind();
      virtual void unbind();
      void doTextures(int frame, Radiant::VideoImage *);
      
      Vector2i planeSize(const Radiant::VideoImage *img, uint i);
      
      Luminous::Texture2D & blankTex() { return m_blankTex; }

    private:

      int m_frame;
      Luminous::Texture2D  m_texIds[3];
      Vector2i             m_texSizes[3];
      Luminous::Texture2D  m_blankTex;
    };

      enum State {
        PLAY,
        PAUSE
      };

      enum {
        HISTOGRAM_POINTS = 256
      };

      VIDEODISPLAY_API ShowGL();
      VIDEODISPLAY_API ~ShowGL();

      VIDEODISPLAY_API bool loadSubTitles(const char * filename, const char * type = 0);

      /// Initialize the file, but does not play it.
      VIDEODISPLAY_API bool init(const char * filename, Resonant::DSPNetwork  * dsp);
      /// Opens the file for playing.
      VIDEODISPLAY_API bool open(const char * filename, Resonant::DSPNetwork  * dsp,
          Radiant::TimeStamp pos = 0);
      /// Stops file playback
      VIDEODISPLAY_API bool stop();

      /// Toggles play/pause state
      VIDEODISPLAY_API bool togglePause();

      State state() const { return m_state; }

      /// Create OpenGL resources
    // bool contextInit();
      /// Free OpenGL resources
      // bool contextCleanup();

      /// Update the video image from reader-thread
      VIDEODISPLAY_API void update();
      /// Render the video to the specified rectangle
      /** 
        @arg topleft Top-left corner of the video image

        @arg bottomright Bottom-right corner of the video image. If
        bottomright = topleft, then the player will use the size of
        the video.

        @arg ransform The coordinates can be optionally transformed
        with the "transform" matrix. */
    VIDEODISPLAY_API void render(Luminous::GLResources * resources,
		Vector2 topleft, Vector2 bottomright,
		const Nimble::Matrix3f * transform = 0,
		Dyslexic::GPUFont * subtitleFont = 0,
		float subTitleSpace = 0);

      /// Pixel size of the video image.
      VIDEODISPLAY_API Nimble::Vector2i size() const;

      Radiant::TimeStamp duration() { return m_duration; }
      Radiant::TimeStamp position() { return m_position; }
      double relativePosition() { return position() / (double) duration(); }

      VIDEODISPLAY_API void seekTo(Radiant::TimeStamp time);
      VIDEODISPLAY_API void seekToRelative(double relative);
      void seekBy(const Radiant::TimeStamp & ts) { seekTo(position() + ts); }

      /// Information on how the frames have been displayed.
      int histogramPoint(int index) const { return m_histogram[index]; } 
      int histogramIndex() const { return m_updates; }

    bool hasSubTitles() { return m_subTitles.size() != 0; }

    private:

      void clearHistogram();
      void getThumbnail(double pos);


      std::string             m_filename;
      VideoIn               * m_video;
      VideoIn::Frame        * m_frame;
      VideoIn::Frame          m_preview;
      Resonant::DSPNetwork  * m_dsp;
      Resonant::DSPNetwork::Item m_dspItem;
      AudioTransfer         * m_audio;
      Radiant::TimeStamp      m_frameTime;
      int                     m_count;
      State                   m_state;
      int                     m_histogram[HISTOGRAM_POINTS];
      uint                    m_updates;

      Radiant::TimeStamp      m_duration;
      Radiant::TimeStamp      m_position;

      Radiant::VideoImage     m_blankDisplay;
      bool                    m_blankReload;
      bool                    m_useBlank;

      SubTitles               m_subTitles;
  };

}

#endif
