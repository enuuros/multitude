#include "ConfigDocument.h"

ConfigDocument::ConfigDocument(void)
{
}

ConfigDocument::~ConfigDocument(void)
{
}
void ConfigDocument::readConfigFile(char *fileName)
{
	string str;

	ifstream input (fileName);
	int depth=0,endDepth=0;
	bool flag=false,atFlag=false,preFlag=true;;
	int k=0;
	while(std::getline(input,str)){
		if(str!="")
			if(parseLine(str)==PARSE_FLAGS::ELEMENT_START)
			{

				string s;
				depth++;
				Element elm,e;
				size_t ps=str.find(",");
				if(ps<10000)
				{

					e.elementName=str.substr(0,ps);
					size_t ps2=str.find("{");
					e.type=str.substr(ps+1,ps2-ps-1);

				}
				else
				{

					e.elementName=str.substr(0,str.find("{")-1);
				}
				//e.elementName=str.substr(0,str.length()-2);
				e.depth=depth;
				elm.Nodes.push_back(e);
				k++;



				while(std::getline(input,s))
				{
					if(s!="")
						if(parseLine(s)==PARSE_FLAGS::ELEMENT_START)
						{
							depth++;
							Element child=Element();
							size_t ps=s.find(",");
							if(ps<10000)
							{

								child.elementName=s.substr(0,ps);
								child.type=s.substr(ps+1,s.find("{")-ps-1);

							}
							else
							{

								child.elementName=s.substr(0,s.find("{")-1);
							}
							child.depth=depth;
							elm.Nodes.push_back(child);
							k++;

						}
						else if(parseLine(s)==PARSE_FLAGS::ATTRIBUTE)
						{
							Attribute att=Attribute();
							size_t pos=s.find("=");

							att.key=s.substr(0,pos);
							att.value=s.substr(pos+1,s.length());

							att.depth=depth;
							elm.Nodes[k-1].Attributes.push_back(att);


						}
						else if(parseLine(s)==PARSE_FLAGS::ELEMENT_END)
						{depth--;


						if(depth==0)
						{
							k=0;

							for(int i=elm.Nodes.size()-1;i>0;i--)
							{
								if(elm.Nodes[i].depth>elm.Nodes[i-1].depth)
								{
									elm.Nodes[i-1].Nodes.push_back(elm.Nodes[i]);

								}
								else
								{

									for(int j=i-2;j>=0;j--)
									{
										if(elm.Nodes[j].depth<elm.Nodes[i].depth && (elm.Nodes[i].depth-elm.Nodes[j].depth)==1 )
										{
											elm.Nodes[j].Nodes.push_back(elm.Nodes[i]);
											break;
										}


									}
								}


							}


							Element el= Element();
							el.Nodes.push_back(elm.Nodes[0]);
							Elements.Nodes.push_back(el);
							elm.Nodes.clear();
							elm.Attributes.clear();
							flag=false;
							atFlag=false;

						}
						}
				}

			}
	}
	input.close();
}
void ConfigDocument::writeConfigFile(char *fileName)
{
	ofstream output(fileName);
	vector<string> ss;
	string aa=getConfigText(Elements,ss);
	//	for(int i=ss.size()-1;i>=0;i--)
	//		output<<ss[i];
	output<<aa;
	output.close();


}
string ConfigDocument::getConfigText(Element e,vector<string> &s)
{
	string str;

	//for(int i=e.Nodes.size()-1;i>=0;i--)

	//	if(e.elementName!="")
	{
		if(e.elementName!="")
		{
			if(e.type=="")
			str+=e.elementName+" {";
			else
				str+=e.elementName+","+e.type+" {";
			str+="\n";
		}

		for(int j=0;j<e.Attributes.size();j++)
		{
			str+=e.Attributes[j].key+"="+e.Attributes[j].value+"\n";

		}
		for(int i=0;i<e.Nodes.size();i++)
			str+=getConfigText(e.Nodes[i],s);

		if(e.elementName!="")
		{
			str+="}";
			str+="\n";
		}
		//s.push_back(str);
	}
	return str;

}
ConfigDocument::PARSE_FLAGS ConfigDocument::parseLine(string line)
{

	if(line[line.length()-1]=='{')
		return PARSE_FLAGS::ELEMENT_START;
	else if(line[line.length()-1]=='}')
		return PARSE_FLAGS::ELEMENT_END;
	else
	{
		size_t pos=line.find("=");
		if(pos>=10000)
			return PARSE_FLAGS::NOT_VALID;
		else
			return PARSE_FLAGS::ATTRIBUTE;
	}
}

