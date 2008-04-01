#include "Task.hpp"

#include <typeinfo>

#include <Radiant/Trace.hpp>

#define TASK_DEFAULT_PRIORITY 0.f

namespace Luminous
{

  Task::Task()
    : m_state(WAITING),
    m_priority(TASK_DEFAULT_PRIORITY),
//    m_canDelete(false),
    m_scheduled(0)
  {}

  Task::~Task()
  {}

  void Task::initialize()
  {}

  void Task::finished()
  {
    Radiant::trace("Task::finished # %s", typeid(*this).name());
  }

}
