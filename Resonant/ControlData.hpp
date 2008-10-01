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

#ifndef RESONANT_CONTROL_DATA_HPP
#define RESONANT_CONTROL_DATA_HPP

#include <Radiant/BinaryData.hpp>

#include <Resonant/Export.hpp>

#include <string>

namespace Resonant {

  class RESONANT_API ControlData : public Radiant::BinaryData
  {

  public:

      ControlData();
  };

}

#endif
