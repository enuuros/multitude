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

#ifndef RESONANT_DSPNETWORK_HPP
#define RESONANT_DSPNETWORK_HPP

#include <Resonant/AudioLoop.hpp>
#include <Resonant/ControlData.hpp>
#include <Resonant/Module.hpp>

#include <Radiant/Mutex.hpp>

#include <Radiant/RefPtr.hpp>

#include <list>
#include <vector>
#include <cassert>

#include <string.h>

namespace Resonant {

  using Radiant::RefPtr;

  class ModuleOutCollect;

  /** Stuff that needs implementing: 

      - When a module is put into the graph (that's running) it can
      end up with getting input from a source that is invalidated by
      later modules. This should be fixed once we get to having
      trouble...
  */

  class RESONANT_API DSPNetwork : public AudioLoop
  {
  public:

    /** Note the lack of destructor. You need to call "clear" manually. */
    class Buf
    {
    public:

      Buf() : m_data(0), m_size(0)
      {}
      
      void allocate(int n) 
      {
	if(n != m_size) {
	  delete [] m_data; 
	  m_data = new float [n];
	  m_size = n;
	}
      }

      void init() { allocate(Module::MAX_CYCLE); }

      void clear() { delete [] m_data; m_data = 0; m_size = 0; }

      float * m_data;
      int     m_size;
    };
    
    class RESONANT_API Connection
    {
    public:
      Connection() : m_channel(0),m_buf(0) { m_moduleId[0] = '\0'; }
      Connection(const char * moduleId, int channel)
        : m_channel(channel),m_buf(0)
      {
	setModuleId(moduleId);
      }
      
      void setModuleId(const char * id)
      {
	if(id)
#ifdef WIN32
    strcpy_s(m_moduleId, id);
#else
        strcpy(m_moduleId, id);
#endif
	else
    m_moduleId[0] = '\0';
      }

      inline bool operator == (const Connection & that) const
      {
        return (strcmp(m_moduleId, that.m_moduleId) == 0) &&
          (m_channel == that.m_channel);
      }

      char        m_moduleId[Module::MAX_ID_LENGTH];
      int         m_channel;
      Buf        *m_buf;
    };

    class RESONANT_API NewConnection
    {
    public:
      NewConnection() : m_sourceChannel(0), m_targetChannel(0) {}

      char        m_sourceId[Module::MAX_ID_LENGTH];
      char        m_targetId[Module::MAX_ID_LENGTH];
      int         m_sourceChannel;
      int         m_targetChannel;
    };

    class RESONANT_API Item
    {
      friend class DSPNetwork;

    public:
      Item();
      ~Item();

      void setModule(Module *m) { m_module = m; }
      Module * module() { return m_module; }

      void setTargetChannel(int channel)
      {
        m_targetChannel = channel;
      }

    private:

      inline void process(int n)
      {
        assert(m_compiled != false);
        float ** in = m_ins.empty() ? 0 : &m_ins[0];
        float ** out = m_outs.empty() ? 0 : &m_outs[0];
        m_module->process(in, out, n);
      }

      void eraseInput(const Connection & c);
      int findInInput(float * ptr) const;
      int findInOutput(float * ptr) const;

      Module * m_module;

      std::vector<Connection> m_inputs;

      std::list<NewConnection> m_connections;
      std::vector<float *> m_ins;
      std::vector<float *> m_outs;
      
      bool m_compiled;
      bool m_done;

      int  m_targetChannel;
    };

    typedef std::list<Item> container;
    typedef container::iterator iterator;

    DSPNetwork();
    virtual ~DSPNetwork();

    bool start(const char * device = 0);

    //int outChannels() { return outParameters().channelCount; }

    void addModule(Item &);
    void markDone(Item &);

    void send(ControlData & control);

    static DSPNetwork * instance();
  
  private:

    virtual int callback(const void *in, void *out,
			 unsigned long framesPerBuffer
//       , const PaStreamCallbackTimeInfo* time,
//			 PaStreamCallbackFlags status
       );
    
    void doCycle(int);

    void checkNewControl();
    void checkNewItems();
    void checkDoneItems();
    void deliverControl(const char * moduleid, const char * commandid, 
			ControlData &);

    bool uncompile(Item &);
    bool compile(Item &);
    bool compile(Item &, int);
    Buf & findFreeBuf(int);
    bool bufIsFree(int, int);
    void checkValidId(Item &);
    Item * findItem(const char * id);
    Module * findModule(const char * id);
    float * findOutput(const char * id, int channel);
    
    container m_items;

    container m_newItems;

    std::vector<Buf> m_buffers;
    
    ModuleOutCollect *m_collect;

    ControlData m_controlData;
    ControlData m_incoming;
    ControlData m_incopy;
    Radiant::MutexAuto m_inMutex;

    char        m_devName[128];
    bool        m_continue;
    long        m_frames;
    int         m_doneCount;

    Radiant::MutexAuto m_newMutex;

    static DSPNetwork * m_instance;
  };

}

#endif
