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

#ifndef RESONANT_MODULE_SAMPLE_PLAYER_HPP
#define RESONANT_MODULE_SAMPLE_PLAYER_HPP

#include <Radiant/FixedStr.hpp>
#include <Radiant/RefPtr.hpp>
#include <Radiant/Thread.hpp>
#include <Radiant/Condition.hpp>

#include <Resonant/Module.hpp>

#include <list>
#include <string>
#include <vector>

#ifdef WIN32
# include <WinPort.h>
#endif

namespace Resonant {

  class DSPNetwork;

  /** Audio sample player for Resonant. */
  class RESONANT_API ModuleSamplePlayer : public Module
  {
  public:


    ModuleSamplePlayer(Application *);
    virtual ~ModuleSamplePlayer();

    virtual bool prepare(int & channelsIn, int & channelsOut);
    virtual void processMessage(const char * address, Radiant::BinaryData *);
    virtual void process(float ** in, float ** out, int n);

    bool addSample(const char * filename, const char * name);

    int findFreeVoice();
    int findSample(const char * );

    void loadSamples();

    /** Adds a few voices that will play an ambount sound background.
        All files in the given directory are loaded looped
        for-ever. In practice one wants to put 3-5 audio files with
        different lengths in the directory. The length of the files
        should be in the 20-30 second range. The end result will be a
        nice ambient background that does not sound like it is
        looping.

        @arg network The DSP network to attach the ambient sound player to.

        @arg directory Where the file are loaded from.

        @arg gain The gain (volume) to give to the background material.

    */

    void createAmbientBackground(DSPNetwork * network,
                                 const char * directory, float gain);

  private:

    class SampleInfo
    {
    public:
      std::string m_name;
      std::string m_filename;
    };

    class Sample
    {
    public:
      Sample();
      ~Sample();

      bool load(const char * filename, const char * name);

      inline const std::vector<float> & data() const { return m_data; }
      const float * buf(unsigned i) const;

      const std::string & name() const { return m_name; }
      /** Number of samples available */
      unsigned available(unsigned pos) const;
      unsigned channels() const;
      unsigned frames() const;

    private:
      class Internal;

      Internal * m_d;

      std::vector<float> m_data;

      std::string m_name;
    };

    class SampleVoice
    {
    public:
      SampleVoice(Sample * s = 0)
        : m_state(INACTIVE), m_gain(1), m_relPitch(1.0f),
          m_sampleChannel(0), m_targetChannel(0), 
          m_sample(s), m_position(0)
      {}

      bool synthesize(float ** out, int n);

      void init(Sample * sample, Radiant::BinaryData * data);

      bool isActive() { return m_state != INACTIVE; }

      void loadFailed() { m_state = INACTIVE; }

      void setSample(Sample * s);

      void clear() { m_state = INACTIVE; m_sample = 0; }

    private:
      enum State {
        INACTIVE,
        WAITING_FOR_SAMPLE,
        PLAYING
      };

      State m_state;

      float m_gain;
      float m_relPitch;
      double m_dpos;

      int      m_sampleChannel;
      int      m_targetChannel;
      bool     m_loop;
      Sample * m_sample;
      unsigned m_position;
    };

    class LoadItem
    {
    public:
      enum { WAITING_COUNT = 64 };

      LoadItem();

      void init(const char * filename, SampleVoice * waiting)
      {
        bzero(m_waiting, sizeof(m_waiting));
        m_waiting[0] = waiting;
        m_name = filename;
        m_free = false;
      }

      inline bool addWaiting(SampleVoice * voice)
      {
        for(int i = 0; i < WAITING_COUNT; i++) {
          if(m_waiting[i] == 0) {
            m_waiting[i] = voice;
            return true;
          }
        }

        return false;
      }

      bool m_free;
      Radiant::FixedStrT<256> m_name;

      SampleVoice * m_waiting[WAITING_COUNT];
    };

    class BGLoader : public Radiant::Thread
    {
    public:

      BGLoader(ModuleSamplePlayer * host);
      ~BGLoader();

      bool addLoadable(const char * filename, SampleVoice * waiting);

    private:

      virtual void childLoop();

      enum { BINS = 256};

      Radiant::Condition m_cond;
      Radiant::MutexAuto m_mutex;

      LoadItem m_loads[BINS];

      ModuleSamplePlayer * m_host;

      volatile bool m_continue;
    };

    bool addSample(Sample * s);

    void dropVoice(unsigned index);

    std::list<SampleInfo> m_sampleList;

    std::vector<Radiant::RefPtr<Sample> > m_samples;

    std::vector<SampleVoice> m_voices;
    std::vector<SampleVoice *> m_voiceptrs;

    unsigned m_channels;
    unsigned m_active;

    BGLoader * m_loader;
  };

}

#endif
