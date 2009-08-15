#include "ConfigAttribute.hpp"
namespace Valuable
{  
ConfigAttribute::ConfigAttribute(void)
{
	depth=0;
}
ConfigAttribute::ConfigAttribute(std::string k, std::string v)
{

	depth=0;
	key=k;
	value=v;

}

ConfigAttribute::~ConfigAttribute(void)
{
}
}