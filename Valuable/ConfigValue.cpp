#include "ConfigValue.hpp"
namespace Valuable
{  
ConfigValue::ConfigValue(void)
{
  m_depth=0;
}
ConfigValue::ConfigValue(std::string k, std::string v)
{

  m_depth=0;
  m_key=k;
  m_value=v;
}

ConfigValue::~ConfigValue(void)
{
}
}
