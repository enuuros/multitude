#include "ConfigElement.hpp"

namespace Valuable
{  
  ConfigElement::ConfigElement(void)
  {
    depth=0;
    type="";
  }

  Attribute *ConfigElement::getAttribute(string key)
  {
    for(int i=0;i<(int) Attributes.size();i++)
      {
	if(Attributes[i].key==key)
	  {
	    return &Attributes[i];
	    break;
	  }

      }
    return 0;

  }

  ConfigElement::~ConfigElement(void)
  {
  }
}
