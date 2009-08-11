#include "Attribute.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
class Element
{
public:
	Element(void);
	~Element(void);
	vector<Attribute> Attributes;
	vector<Element> Nodes;
	string type;
	int depth;
	string elementName;


};
