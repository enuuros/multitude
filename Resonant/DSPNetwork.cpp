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

#include "DSPNetwork.hpp"

#include "ModuleOutCollect.hpp"

#include <Radiant/FixedStr.hpp>
#include <Radiant/Trace.hpp>

#include <algorithm>
#include <typeinfo>

#ifdef WIN32
#include <WinPort.h>
#endif

namespace Resonant {

  using namespace Radiant;
  
  DSPNetwork::Item::Item()
    : m_module(0),
      m_compiled(false),
      m_done(false)
  {}

  DSPNetwork::Item::~Item()
  {}

  void DSPNetwork::Item::eraseInput(const Connection & c)
  {
    std::vector<Connection>::iterator it =
      std::find(m_inputs.begin(), m_inputs.end(), c);

    if(it != m_inputs.end())
      m_inputs.erase(it);
  }

  int DSPNetwork::Item::findInInput(float * ptr) const
  { 
    for(uint i = 0; i < m_ins.size(); i++)
      if(m_ins[i] == ptr)
	return i;
    return -1;
  }

  int DSPNetwork::Item::findInOutput(float * ptr) const
  { 
    for(uint i = 0; i < m_outs.size(); i++)
      if(m_outs[i] == ptr)
	return i;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////


  DSPNetwork * DSPNetwork::m_instance = 0;

  DSPNetwork::DSPNetwork()
    : m_continue(false),
      m_doneCount(0)
  {
    m_devName[0] = 0;
    m_collect = new ModuleOutCollect(0, this);
    m_collect->setId("outcollect");

    Item tmp;
    tmp.m_module = m_collect;

    m_newItems.push_back(tmp);

    if(!m_instance)
      m_instance = this;
  }

  DSPNetwork::~DSPNetwork()
  {
    if(m_instance == this)
      m_instance = 0;

    for(uint i = 0; i < m_buffers.size(); i++)
      m_buffers[i].clear();
  }
  
  bool DSPNetwork::start(const char * device)
  {
    if(isRunning())
      return false;

    if(device)
      strcpy(m_devName, device);
    else
      m_devName[0] = '\0';

    // m_continue = true;
    
    return startReadWrite(44100, 2);
  }


  void DSPNetwork::addModule(Item & i)
  {
    Radiant::Guard g( & m_newMutex);

    m_newItems.push_back(i);
  }


  void DSPNetwork::markDone(Item & i)
  {
    Radiant::Guard g( & m_newMutex);
    Item * it = findItem(i.m_module->id());

    if(it) {
      it->m_done = true;
      m_doneCount++;
    }
    else
      error("DSPNetwork::markDone # Failed for \"%s\"", i.m_module->id());
  }

  void DSPNetwork::send(ControlData & control)
  {
    Radiant::Guard g( & m_inMutex);
    m_incoming.append(control);
  }

  int DSPNetwork::callback(const void *in, void *out,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* time,
			   PaStreamCallbackFlags status)
  {
    (void) in;
    (void) time;
    (void) status;

    /* printf("st = %lf, dact = %lf\n",
       m_streamInfo->outputLatency, time->outputBufferDacTime);*/
    // Pa_GetStreamTime(m_stream), time->outputBufferDacTime);

    doCycle(framesPerBuffer);
    const float * res = m_collect->interleaved();
    assert(res != 0);
    memcpy(out, res, 4 * framesPerBuffer * outParameters().channelCount);

    return paContinue;
  }
  
  void DSPNetwork::doCycle(int framesPerBuffer)
  {
    const int cycle = framesPerBuffer;

    checkNewItems();
    checkNewControl();

    for(iterator it = m_items.begin(); it != m_items.end(); it++) {
      Item & item = (*it);
      // Module * m = item.m_module;
      // trace("DSPNetwork::doCycle # Processing %p %s", m, typeid(*m).name());
      item.process(cycle);
    }

    checkDoneItems();

    // const float * res = m_collect->interleaved();

    // m_device.writeInterleaved(res, cycle);
  }

  void DSPNetwork::checkNewControl()
  {
    {
      Radiant::Guard g( & m_inMutex);
      m_incopy = m_incoming;
      m_incoming.rewind(); 
    }

    int sentinel = m_incopy.pos();

    m_incopy.rewind();

    char buf[512];
    
    FixedStrT<512> id;

    while(m_incopy.pos() < sentinel) {
      buf[0] = 0;
      
      if(!m_incopy.readString(buf, 512)) {
	error("DSPNetwork::checkNewControl # Could not read string");
	continue;
      }

      const char * slash = strchr(buf, '/');
      const char * command;

      if(!slash) {
	id = buf;
	command = 0;
      }
      else {
	id.copyn(buf, slash - buf);
	command = slash + 1;
      }
      
      deliverControl(id, command, m_incopy);
    }
  }

  void DSPNetwork::checkNewItems()
  {
    while(m_newItems.size()) {

      trace("DSPNetwork::checkNewItems # Next ");

      if(!m_newMutex.tryLock())
        return;

      Radiant::ReleaseGuard g( & m_newMutex);

      Item item = m_newItems.front();
      checkValidId(item);
      m_newItems.pop_front();
      m_items.push_front(item);
      const char * type = typeid(* item.m_module).name();

      Item * itptr = & (*m_items.begin());


      if(!compile(*itptr, 0)) {
        error("DSPNetwork::checkNewItems # Could not add module %s", type);
        m_items.pop_front();
      }
      else {
        trace("DSPNetwork::checkNewItems # Added a new module %s", type);
	
	if(itptr->m_module == m_collect)
	  continue;

	int mchans = itptr->m_outs.size();
	int outchans = 2; // hardware output channels
	const char * id = itptr->m_module->id();
	if(mchans) {

	  /* Add mappings for the new module, so that it is
	     heard. Realistically, this behavior should be overridable
	     as needed, now one cannot really make too clever DSP
	     networks.
	   */
	  Item * oi = findItem(m_collect->id());
	  
	  if(!oi)
	    Radiant::fatal("DSPNetwork::checkNewItems # No collector \"%s\"",
			  m_collect->id());

	  for(int i = 0; i < outchans; i++) {
	    Connection conn;
	    conn.setModuleId(id);
	    conn.m_channel = i % mchans;
	    oi->m_inputs.push_back(conn);

	    m_controlData.rewind();
	    m_controlData.writeString(id); // Source id
	    m_controlData.writeInt32(i % mchans);// Source module output channel
	    m_controlData.writeInt32(i % outchans); // Target channels
	    m_controlData.rewind();
	    m_collect->control("newmapping", & m_controlData);
	  }
	  compile( * oi);
	  trace("DSPNetwork::checkNewItems # Compiled out collector", type);
	}
      }
    }
  }
  
  void DSPNetwork::checkDoneItems()
  {
    if(!m_newMutex.tryLock())
      return;

    Radiant::ReleaseGuard g( & m_newMutex);

    if(!m_doneCount)
      return;

    for(iterator it = m_items.begin(); it != m_items.end(); ) {
      Item & item = (*it);
      if(item.m_done) {
        uncompile(item);
        
        item.m_module->stop();
 
	trace("DSPNetwork::checkDoneItems # Stopped %p", item.m_module);
       iterator tmp = it;
        tmp++;
        m_items.erase(it);
        it = tmp;
      }
      else
        it++;
    }

    m_doneCount = 0;
  }
  
  void DSPNetwork::deliverControl(const char * moduleid,
				  const char * commandid, 
				  ControlData & data)
  {
    for(iterator it = m_items.begin(); it != m_items.end(); it++) {
      Module * m = (*it).m_module;
      if(strcmp(m->id(), moduleid) == 0) {
	m->control(commandid, & data);
	return;
      }
    } 
    error("DSPNetwork::deliverControl # No module \"%s\"", moduleid);
  }


  bool DSPNetwork::uncompile(Item & item)
  {
    Module * m = item.m_module;

    if(m == m_collect)
      return true;

    int mchans = item.m_outs.size();
    int outchans = 2; // hardware output channels
    const char * id = m->id();

    if(mchans) {
      Item * oi = findItem(m_collect->id());

      if(!oi)
        Radiant::fatal("DSPNetwork::checkNewItems # No collector \"%s\"",
                       m_collect->id());
      
      for(int i = 0; i < outchans; i++) {
        Connection conn;
        conn.setModuleId(id);
        conn.m_channel = i;
        oi->eraseInput(conn);
        
        m_controlData.rewind();
        m_controlData.writeString(id);
        m_controlData.writeInt32(i);
        m_controlData.writeInt32(i % outchans);
        m_controlData.rewind();
        m_collect->control("removemapping", & m_controlData);
      }
      compile( * oi);
      trace("DSPNetwork::uncompile # uncompiled \"%s\"", id);
    }

    return true;
  }

  bool DSPNetwork::compile(Item & item)
  {
    int i = 0;

    for(iterator it = m_items.begin(); it != m_items.end(); it++) {
      if(& (*it) == & item)
	return compile(item, i);
      i++;
    }
    
    Radiant::error("DSPNetwork::compile # Failed to find something to compile");

    return false;
  }

  bool DSPNetwork::compile(Item & item, int location)
  {
    int i = 0;
    int ins = item.m_inputs.size();
    int outs = ins;
    
    std::list<NewConnection>::iterator conit;

    std::list<Item *> affected;

    for(conit = item.m_connections.begin(); conit != item.m_connections.end();
        conit++) {
      NewConnection & nc = *conit;
      if(strcmp(nc.m_targetId, item.m_module->id()) == 0) {
        item.m_inputs.push_back(Connection(nc.m_sourceId,
					   nc.m_sourceChannel));
	trace("Item[%d].m_inputs[%d] = [%d,%d]", location, i,
	      nc.m_sourceId, nc.m_sourceChannel);
      }
      i++;
    }

    item.m_module->prepare(ins, outs);

    assert(ins == (int) item.m_inputs.size());

    item.m_ins.resize(ins);
    bzero( & item.m_ins[0], item.m_ins.size() * sizeof(float *));

    item.m_outs.resize(outs);
    bzero( & item.m_outs[0], item.m_outs.size() * sizeof(float *));

    for(i = 0; i < ins; i++) {
      Connection & conn = item.m_inputs[i];
      float * ptr = findOutput(conn.m_moduleId, conn.m_channel);
      item.m_ins[i] = ptr;
      trace("Item[%d].m_ins[%d] = %p", location, i, ptr);
    }

    for(i = 0; i < outs; i++) {
      if(item.m_outs[i] == 0) {
	Buf & b = findFreeBuf(location);
	item.m_outs[i] = b.m_data;
	trace("Item[%d].m_outs[%d] = %p", location, i, b.m_data);
      }
    }

    item.m_compiled = true;
    
    Module * m = item.m_module;

    trace("DSPNetwork::compile # compiled %p %s", m, typeid(*m).name());

    return true;
  }

  DSPNetwork::Buf & DSPNetwork::findFreeBuf(int location)
  {
    uint s = m_buffers.size();

    for(uint i = 0; i < s; i++) {
      if(bufIsFree(i, location))
	return m_buffers[i];
    }

    m_buffers.resize(s + 1);

    m_buffers[s].init();
    
    return m_buffers[s];
  }

  bool DSPNetwork::bufIsFree(int channel, int location)
  {
    iterator it = m_items.begin();
    int i;

    for(i = 0; i < location; i++)
      it++;

    float * ptr = m_buffers[channel].m_data;
    
    if((*it).findInOutput(ptr) >= 0)
      return true;
    
    iterator other = it;

    for(uint i = 0; i < m_items.size(); i++) {
      other++;
      if(other == m_items.end())
        other = m_items.begin();

      Item & item = (*other);

      if(item.findInInput(ptr) >= 0)
	return false;
      else if(item.findInOutput(ptr) >= 0)
	return true;
    }

    return true;
  }

  DSPNetwork::Item * DSPNetwork::findItem(const char * id)
  {
    for(iterator it = m_items.begin(); it != m_items.end(); it++) {
      Item & item = (*it);
      if(strcmp(item.m_module->id(), id) == 0) {
        return & item;
      }
    }
    return 0;
  }

  void DSPNetwork::checkValidId(Item & it)
  {
    char buf[32];
    int index = 0;

    Module * m = it.m_module;
    
    if(strlen(m->id()) == 0) {
      sprintf(buf, "%p", m);
      m->setId(buf);
      index++;
    }
    
    while(findItem(m->id())) {
      if(!index)
	sprintf(buf, "%p", m);
      else 
	sprintf(buf, "%p-%.4d", m, index);
      m->setId(buf);
      index++;
    }
  }

  Module * DSPNetwork::findModule(const char * id)
  {
    Item * item = findItem(id);

    if(!item)
      return 0;

    return item->m_module;
  }

  float * DSPNetwork::findOutput(const char * id, int channel)
  {
    Item * item = findItem(id);

    if(!item)
      return 0;

    if(item->m_outs.size() >= (uint) channel)
      return item->m_outs[channel];
    return 0;
  }
}
