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

#ifndef LUMINOUS_LOADABLE_HPP
#define LUMINOUS_LOADABLE_HPP

#include "Task.hpp"
#include <string>

namespace Luminous
{
  class BGThread;

  class Loadable : public Task
  {
    public:
      Loadable(const std::string& filepath);
      virtual ~Loadable();

      /// @todo remove
      const std::string& filename() const { return m_filepath; }

      void setCanBeDeleted(bool v) { m_canDelete = v; }
      bool canBeDeleted() const { return m_canDelete; }

      /// @todo remove these with Task-approach
      void setUserData(void * p) { m_userData = p; }
      void * userData() { return m_userData; }  

      friend class BGThread; 

    protected:
      std::string m_filepath;
      bool m_canDelete;

      BGThread* m_loader;

      void * m_userData;
  };

}

#endif
