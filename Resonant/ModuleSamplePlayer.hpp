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

#ifndef RESONANT_MODULE_SAMPLE_PLAYER_HPP
#define RESONANT_MODULE_SAMPLE_PLAYER_HPP

#include <Radiant/RefPtr.hpp>

#include <Resonant/Module.hpp>

#include <sndfile.h>

#include <list>
#include <string>
#include <vector>

namespace Resonant {

  class ModuleSamplePlayer : public Module
  {
  public:

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
      inline const float * buf(unsigned i) const
      { return &m_data[i * m_info.channels]; }

      const SF_INFO & info() const { return m_info; }

    protected:
      SF_INFO m_info;

      std::vector<float> m_data;
      
      std::string m_name;
    };

    class SampleVoice
    {
    public:
      SampleVoice(Sample * s = 0) : m_sample(s), m_position(0) {}
      
      bool synthesize(float ** out, int n);
      
    protected:
      Sample * m_sample;
      unsigned m_position;
    };

    ModuleSamplePlayer(Application *);
    virtual ~ModuleSamplePlayer();

    virtual bool prepare(int & channelsIn, int & channelsOut);
    virtual void control(const char * address, ControlData *);
    virtual void process(float ** in, float ** out, int n);

    bool addSample(const char * filename, const char * name);

    void loadSamples();
    
  protected:
    
    void dropVoice(unsigned index);

    std::list<SampleInfo> m_sampleList;

    std::vector<Radiant::RefPtr<Sample> > m_samples;

    std::vector<SampleVoice> m_voices;


    unsigned m_channels;
    unsigned m_active;
  };

}

#endif
