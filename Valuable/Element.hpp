#include <Valuable/Export.hpp>
#include "Attribute.hpp"
#include <iostream>
#include <vector>
#include <string>
namespace Valuable
{  
using namespace std;
class VALUABLE_API Element
{
public:
	Element(void);
	virtual ~Element(void);
	vector<Attribute> Attributes;
	vector<Element> Nodes;
	Attribute *getAttribute(string key);
	string type;
	int depth;
	string elementName;


};
}