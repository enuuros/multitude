#include <Valuable/Export.hpp>
#include "Element.hpp"
#include <fstream>
namespace Valuable
{  
class VALUABLE_API ConfigDocument
{
public:
	 ConfigDocument(void);
	 virtual ~ConfigDocument(void);

	 void readConfigFile(char *fileName);
	 void writeConfigFile(char *fileName);
	 Element *getElement(string elementName);
	  Element *getElement(string key,string value);
	 Element Elements;
	
	
private:
	void loadElement(string str);
	void TrimSpaces( std::string& str);
	void loadAttribute(string key,string val);
	string getConfigText(Element e,vector<string> &ss);
	Element *findElement(Element &e,string elementName,bool &found);
	 Element *findElement(Element &e,bool &found,string key,string value);
	enum PARSE_FLAGS
	{
		ELEMENT_START,ELEMENT_END,ATTRIBUTE,NOT_VALID

	};
	PARSE_FLAGS parseLine(string line);
};
}