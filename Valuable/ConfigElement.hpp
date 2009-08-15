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
#include "ConfigValue.hpp"
#include <iostream>
#include <vector>
#include <string>
namespace Valuable
{  
  using namespace std;
  class VALUABLE_API ConfigElement
  {
  public:
    ConfigElement();
    virtual ~ConfigElement();

    vector<ConfigValue> ConfigValues;
    vector<ConfigElement> Nodes;
    ConfigValue *getConfigValue(string key);
    string type;
    int depth;
    string elementName;
  };
}

#endif

