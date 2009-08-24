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

#ifndef RESONANT_MODULE_SPLITTER_HPP
#define RESONANT_MODULE_SPLITTER_HPP

#include <Nimble/Ramp.hpp>
#include <Nimble/Vector2.hpp>

#include <Radiant/RefObj.hpp>

#include <Resonant/Module.hpp>

#include <vector>

namespace Resonant {

  /** Split audio signals to multiple outputs. */
  class ModuleSplitter : public Module
  {
  public:
    ModuleSplitter(Application *);
    virtual ~ModuleSplitter();

    virtual bool prepare(int & channelsIn, int & channelsOut);
    virtual void control(const char *, Radiant::BinaryData *);
    virtual void process(float ** in, float ** out, int n);

    /** Creates a loudspeaker/headphone setup for full-HD displays.

        One source on the left at [0, 540], one source at right, at
        [1920, 540]. */

    void makeFullHDStereo();

  protected:

    void setSourceLocation(unsigned index, Nimble::Vector2 location);

    class LoudSpeaker
    {
    public:
      LoudSpeaker()
	: m_location(0, 0)
      {}

      Nimble::Vector2 m_location; // PixelLocation.
    };

    class Pipe
    {
    public:
      Pipe()
	: m_to(0)
      {
	m_ramp.reset(0.0);
      }
            
      bool done() {return (m_ramp.left() == 0) && (m_ramp.value() < 1.0e-4f);}
      
      Nimble::Rampf m_ramp;
      
      unsigned m_to;
    };

    enum { PIPES_PER_SOURCE = 6 };

    class Source
    {
    public:
      Source() : m_location(0, 0), m_updates(0), m_index(0) {}
      
      Nimble::Vector2 m_location;
      bool m_updates;
      unsigned  m_index;

      Pipe m_pipes[PIPES_PER_SOURCE];
    };

    typedef std::vector<Radiant::RefObj<Source> > Sources;
    typedef std::vector<LoudSpeaker> LoudSpeakers;

    Sources      m_sources;
    LoudSpeakers m_speakers;

    int m_outChannels;
    unsigned m_counter;

    float m_maxRadius;
  };

}

#endif
