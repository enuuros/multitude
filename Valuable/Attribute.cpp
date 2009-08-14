#include "Attribute.hpp"
namespace Valuable
{  
Attribute::Attribute(void)
{
	depth=0;
}
Attribute::Attribute(std::string k, std::string v)
{

	depth=0;
	key=k;
	value=v;

}

Attribute::~Attribute(void)
{
}
}