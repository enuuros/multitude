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

#ifndef RESONANT_MODULE_HPP
#define RESONANT_MODULE_HPP

#include <Resonant/Export.hpp>

namespace Resonant {

  class ControlData;
  class Application;

  class RESONANT_API Module
  {
  public:

    enum {
      MAX_ID_LENGTH = 256,
      MAX_CYCLE = 1024
    };

    Module(Application *);
    virtual ~Module();

    virtual bool prepare(int & channelsIn, int & channelsOut);
    virtual void control(const char * address, ControlData *);
    virtual void process(float ** in, float ** out, int n) = 0;
    virtual bool stop();
    
    void setId(const char *);
    const char * id() { return m_id; }
    
  private:
    Application * m_application;
    char m_id[MAX_ID_LENGTH];
  };

}

#endif
