#include "Element.hpp"
namespace Valuable
{  
Element::Element(void)
{
	depth=0;
	type="";
}
Attribute *Element::getAttribute(string key)
{
	for(int i=0;i<Attributes.size();i++)
	{
		if(Attributes[i].key==key)
		{
			return &Attributes[i];
			break;
		}

	}
	return 0;

}
Element::~Element(void)
{
}
}