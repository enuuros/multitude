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

#ifndef LUMINOUS_BGTHREAD_HPP
#define LUMINOUS_BGTHREAD_HPP

#include <Luminous/Loadable.hpp>
#include <list>
#include <map>
#include <Radiant/Thread.hpp>
#include <Radiant/Mutex.hpp>
#include <Radiant/Condition.hpp>

namespace Luminous
{

  class BGThread : public Radiant::Thread
  {

  public:
    BGThread();
    virtual ~BGThread();

    /// @todo rename to addTask(Task * )
    /// Adds a task to be executed
    virtual void startLoading(Loadable* loadable);
 
    /// Stop the thread and wait for it to terminate
    virtual void stop();

    /// @todo remove, add eraseTask(Task *)     
    virtual void setKeepLoaded(bool keep) { m_keepLoaded = keep; }

    /// @todo rename to setPriority(Task *, Priority )
    virtual void hint(Loadable* loadable, Priority p);

    typedef std::multimap<Priority, Loadable* > container;
    typedef std::pair<Priority, Loadable* > contained;

  protected:
    /// @deprecated
    virtual void loopLogic();
    virtual void childLoop();

    /// @todo implement similar logic in Task
    /// @deprecated
    virtual void finishedLoading(Loadable * loadable);
    /// @deprecated
    virtual void startedLoading(Loadable * loadable);

    Radiant::MutexAuto m_mutex;
    Radiant::MutexAuto m_mutexWait;
    Radiant::Condition m_wait;

    container m_toLoad;
    std::list<Loadable*> m_loaded;
    
    bool m_continue;
    bool m_keepLoaded;
  };

}

#endif

