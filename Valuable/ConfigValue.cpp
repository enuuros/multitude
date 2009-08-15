#include "ConfigValue.hpp"
namespace Valuable
{  
ConfigValue::ConfigValue(void)
{
	depth=0;
}
ConfigValue::ConfigValue(std::string k, std::string v)
{

	depth=0;
	key=k;
	value=v;

}

ConfigValue::~ConfigValue(void)
{
}
}