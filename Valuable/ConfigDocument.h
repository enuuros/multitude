#include "Element.h"
#include <fstream>
class ConfigDocument
{
public:
	ConfigDocument(void);
	~ConfigDocument(void);

	void readConfigFile(char *fileName);
	void writeConfigFile(char *fileName);
	Element Elements;
	
	
private:
	void loadElement(string str);
	
	void loadAttribute(string key,string val);
	string getConfigText(Element e,vector<string> &ss);
	
	enum PARSE_FLAGS
	{
		ELEMENT_START,ELEMENT_END,ATTRIBUTE,NOT_VALID

	};
	PARSE_FLAGS parseLine(string line);
};
