#include "ConfigElement.hpp"

namespace Valuable
{  
  ConfigElement::ConfigElement(void)
  {
    depth=0;
    type="";
  }

  ConfigAttribute *ConfigElement::getConfigAttribute(string key)
  {
    for(int i=0;i<(int) ConfigAttributes.size();i++)
      {
	if(ConfigAttributes[i].key==key)
	  {
	    return &ConfigAttributes[i];
	    break;
	  }

      }
    return 0;

  }

  ConfigElement::~ConfigElement(void)
  {
  }
}
