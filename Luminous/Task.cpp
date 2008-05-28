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
