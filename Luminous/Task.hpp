#ifndef LUMINOUS_TASK_HPP
#define LUMINOUS_TASK_HPP

namespace Luminous
{
  typedef unsigned char Priority;

  class Task
  {
    public:
      Task(): m_state(INITIALIZED), m_priority(255) {}
      virtual ~Task() {}

      enum State
      {
        INITIALIZED,
        PROCESSING,
        DONE,
        FAILURE
      };

      Priority priority() const { return m_priority; }

      State state() const { return m_state; }
      virtual void doTask() = 0;

    protected:
      State m_state;
      Priority m_priority;
  };

}

#endif
