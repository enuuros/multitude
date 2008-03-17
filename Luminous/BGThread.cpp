/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/BGThread.hpp>
#include <cassert>

namespace Luminous
{
  BGThread::BGThread():
    m_continue(true),
    m_keepLoaded(true)
  {}
  
  static void g_deletePred1(Loadable* x)
  {
    delete x;
  }

  static void g_deletePred2(BGThread::contained c)
  {
    delete c.second;
  }


  BGThread::~BGThread()
  {
    stop();

    /// @todo Free resources, should we do this?
    for_each(m_loaded.begin(), m_loaded.end(), g_deletePred1);    
    for_each(m_toLoad.begin(), m_toLoad.end(), g_deletePred2);
  }

  void BGThread::startLoading(Loadable* loadable)
  {
    assert(loadable);

    m_mutex.lock();
    m_toLoad.insert(contained(loadable->priority(), loadable));
    m_mutex.unlock();

    m_wait.wakeAll();
  }

  void BGThread::stop()
  {
    m_continue = false;

    if(isRunning()) 
    {
      m_wait.wakeAll();
      waitEnd();
    }
  }

  void BGThread::hint(Loadable* ref, Priority p)
  {
    m_mutex.lock();

    container::iterator beg = m_toLoad.find(ref->priority());
    container::iterator end = m_toLoad.upper_bound(ref->priority());

    container::iterator it;
    for(it = beg; it != end; it++) {
      if(it->second == ref) break;
    }

    if(it != end) {
      m_toLoad.erase(it);

      ref->m_priority = p;
      m_toLoad.insert(contained(p, ref));
    }

    m_mutex.unlock();
  }

  static bool g_deleteMarkedPred(Loadable* x)
  {
    if(x->canBeDeleted()) {
      delete x;
      return true;
    }
   
    return false;
  }

  void BGThread::childLoop()
  {
    while(m_continue) {

      // Check for files to load
      Loadable* ref = 0;

      m_mutex.lock();

      loopLogic();
 
      if(!m_toLoad.empty()) {
        container::iterator it = m_toLoad.begin();
        
        ref = it->second;
        m_toLoad.erase(it);
      }

      m_mutex.unlock();

      // Load the new file, if available
      if(ref) {

        if(ref->canBeDeleted()) {
          //delete ref;
          //ref = 0;
        } else {
          bool first = (ref->state() == Task::INITIALIZED);

          ref->doTask();
  
          if(first && ref->state() == Task::PROCESSING) 
            startedLoading(ref);

          // Did the loading complete?
          if(ref->state() == Task::DONE) {
            finishedLoading(ref);

            if(m_keepLoaded) {
              m_loaded.push_back(ref);
            }
          } else {
            // If we are still loading, push the item to the back of the given
            // priority so that other items with the same priority will be loaded 
            // before loading this one again
            m_mutex.lock();
            m_toLoad.insert(contained(ref->priority(), ref));
            m_mutex.unlock();
          }
        }
      }

      m_loaded.remove_if(g_deleteMarkedPred);

      // Wait for new requests to appear:
      while(m_toLoad.empty() && m_continue) {
        m_mutexWait.lock();
        m_wait.wait(m_mutexWait);
        m_mutexWait.unlock();
      }
    }
  }

  void BGThread::finishedLoading(Loadable * )
  {}

  void BGThread::loopLogic()
  {}

  void BGThread::startedLoading(Loadable *)
  {}

}
