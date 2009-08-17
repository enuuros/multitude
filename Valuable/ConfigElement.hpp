/* COPYRIGHT
 *
 * This file is part of Valuable.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Valuable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUABLE_ELEMENT_HPP
#define VALUABLE_ELEMENT_HPP

#include <Valuable/Export.hpp>
#include <Valuable/ConfigValue.hpp>

#include <iostream>
#include <vector>
#include <string>

namespace Valuable
{  

  class VALUABLE_API ConfigElement
  {
  public:
    ConfigElement();
    virtual ~ConfigElement();

    ConfigValue *getConfigValue(std::string key);

    void clear();

  private:

    friend class ConfigDocument;

    std::vector<ConfigValue>   m_values;
    std::vector<ConfigElement> m_nodes;
    std::string                m_type;
    int                        m_depth;
    std::string                m_elementName;
  };
}

#endif

