#include "ConfigElement.hpp"

namespace Valuable
{

  ConfigElement::ConfigElement(void)
  {
    m_depth = 0;
    m_type = "";
  }

  ConfigValue * ConfigElement::getConfigValue(std::string key)
  {
    for(int i = 0; i < (int) m_values.size(); i++) {

      if(m_values[i].m_key == key) {
	return & m_values[i];
	break;
      }
    }

    return 0;
  }

  ConfigElement::~ConfigElement(void)
  {
  }
}
