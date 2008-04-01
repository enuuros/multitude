#ifndef LUMINOUS_TASK_HPP
#define LUMINOUS_TASK_HPP

#include <Patterns/NotCopyable.hpp>

#include <Radiant/TimeStamp.hpp>

namespace Luminous
{
  typedef float Priority;
 
  class Task : Patterns::NotCopyable
  {
    /// @todo add timestamp to control the frequency of running a task (ie. once
    /// every five seconds)

    public:
      Task();
      virtual ~Task();

      /// State of the task
      enum State
      {
        WAITING,      ///< Task is waiting in queue to be processed
        RUNNING,      ///< Task is currently running
        DONE          ///< Task has finished
      };

      /// Get the current priority of the task
      Priority priority() const { return m_priority; }

      /// Get the current state of the task
      State state() const { return m_state; }

      /// The actual work the task does should be implemented in here. Override
      /// in the derived class
      virtual void doTask() = 0;
  
      /// Return a timestamp for the next execution of the task
      Radiant::TimeStamp scheduled() const { return m_scheduled; }
      /// Schedule the next execution time for this task
      void schedule(Radiant::TimeStamp ts) { m_scheduled = ts; }

      /// @internal 
      //bool canBeDeleted() const { return m_canDelete; }

    protected:
       /// Initialize the task. Called by BGThread before the task is processed
      virtual void initialize();
      /// Finished the task. Called by BGThread after the task has been
      /// processed
      virtual void finished();
      State m_state;
      Priority m_priority;
      //bool m_canDelete;

      Radiant::TimeStamp m_scheduled;

      friend class BGThread;
  };

}

#endif
