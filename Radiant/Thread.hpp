/* COPYRIGHT
 *
 * This file is part of Diva.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Diva.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef DIVA_THREAD_HPP
#define DIVA_THREAD_HPP

#define PTHREAD

#include <pthread.h>

namespace Radiant {

  class Mutex;

  /// Platform-independent threading
  /** This class is used by inheriting it and overriding the virtual
      method childLoop().

      At the moment there is only POSIX-threads -based implementation,
      that works on various UNIX-like systems.
      */
  class Thread
  {
  public:
    /// Thread id type.
    /** On most systems this is some kind of integer value. */
    typedef pthread_t id_t;

    /// The id of the calling thread
    static id_t myThreadId() { return pthread_self(); }

    /** Construct a thread structure. The thread is NOT activated by this
	method. */
    Thread();

    /// Destructor
    /** The thread must be stopped before this method is
	called. Thread cannot be terminated within the destructor, as
	the inheriting class that implements the virtual childLoop
	function does not exist any more (its desctructor is called
	before this function). */
    virtual ~Thread();

    /** Starts the thread in either system-level or process-level
	scope. If the OS fails to deliver the desired thread type the
	other kind is silently used instead. This method uses the other
	two "run" methods */
    bool run(bool prefer_system = true);
  
    /// Runs a system-level thread.
    /** System-level threads usually have separate process ids for
	each thread. On some platforms (such as Linux), this is the
	only kind of thread. On some platforms (such as IRIX) you need
	special privileges to run system-level threads.*/
    bool runSystem();
    /// Runs a process-level thread.
    /** Process-level threads work inside one process id and
	usually. This thread type is supposed to offer very good
	performance. Some people (such as Linus Torvalds) do not share
	this view. */
    bool runProcess();

    /** Waits until thread is finished. This method does nothing to
	kill the thread, it simply waits until the thread has run its
	course. */
    bool waitEnd();

    /** Kills the thread. A violent way to shut down a thread. You
	should only call this method in emergency situations. May result
	in application crash and other minor problems.*/
    void kill();

    /// Returns true if the thread is running.
    bool isRunning();

    /** Drive some self tests. */
    static void test();

  protected:
    /// Exits the the calling thread.
    void threadExit();

    /// Calls childLoop.
    void mainLoop();

    /** The actual contents of the thread. You need to override this to
	add functionality to your software. */
    virtual void childLoop() = 0;

  private:
    static void *entry(void *);

    pthread_t       m_thread;

    int             m_state;

    static bool m_threadDebug;
    static bool m_threadWarnings;
  };

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  /** Status of available thread types. 
    
      This class can be used that certain thread types do work.

      Only static methods.
  */
  class ThreadAvailable
  {
  public:
    ThreadAvailable() {}
    ~ThreadAvailable() {}

    /// Returns true if system-level threads seem to work.
    static bool systemLevelWorks();
    /// Returns true if process-level threads seem to work.
    static bool processLevelWorks();
    /** Performs checks for different thread models. You do not need to
	call this method explicitly. If this method has not been called
	prior to "systemLevelWorks" etc it will be called to check the
	values. */
    static void check();
  protected:
    static bool m_systemLevel, m_processLevel, m_defined;

  };

  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  /// A simple thread test that does nothing important.
  class ThreadTest : public Thread
  {
  public:
    ThreadTest();
    ~ThreadTest() {}

    int count();
    void requestEnd();
  
  protected:
    void childLoop();

    Mutex *m_mutex;

    bool m_continue;
    int  m_counter;
  };

}

#endif
