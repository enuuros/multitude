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

//#include <Luminous/Loadable.hpp>
#include <Luminous/Task.hpp>

#include <Radiant/Thread.hpp>
#include <Radiant/Mutex.hpp>
#include <Radiant/Condition.hpp>

#include <list>
#include <map>

namespace Luminous
{

  class BGThread : public Radiant::Thread
  {

  public:
    BGThread();
    virtual ~BGThread();

    /// Add a task to be executed
    virtual void addTask(Task * task);
 
    /// Queue a task for deletion. The time of deletion is not guaranteed to be
    /// immediate
    //virtual void markForDeletion(Task * task);

    /// Stop the thread and wait for it to terminate
    virtual void stop();

    /// Change the priority of a task
    virtual void setPriority(Task * task, Priority p);

    typedef std::multimap<Priority, Task * > container;
    typedef std::pair<Priority, Task * > contained;

  protected:
    virtual void childLoop();

    Task * pickNextTask(Radiant::TimeStamp & wait);

    Radiant::MutexAuto m_mutex;
    Radiant::MutexAuto m_mutexWait;
    Radiant::Condition m_wait;

    container m_taskQueue;
    
    bool m_continue;
  };

}

#endif

