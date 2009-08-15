#include "ConfigElement.hpp"

namespace Valuable
{

  ConfigElement::ConfigElement(void)
  {
    depth=0;
    type="";
  }

  ConfigValue * ConfigElement::getConfigValue(std::string key)
  {
    for(int i=0;i<(int) ConfigValues.size();i++)
      {
	if(ConfigValues[i].key==key)
	  {
	    return &ConfigValues[i];
	    break;
	  }

      }
    return 0;

  }

  ConfigElement::~ConfigElement(void)
  {
  }
}
