#include "ConfigDocument.hpp"

#include <fstream>

namespace Valuable
{  

  ConfigDocument::ConfigDocument(void)
  {
  }

  ConfigDocument::~ConfigDocument(void)
  {
  }

  void ConfigDocument::readConfigFile(char *fileName)
  {
    std::string str;

    std::ifstream input (fileName);

    int depth=0;
    bool flag=false,atFlag=false;

    int k=0;
    while(std::getline(input,str)) {

      if(str == "")
	continue;

      if(parseLine(str)==ELEMENT_START) {

	std::string s;
	depth++;
	ConfigElement elm,e;
	size_t ps=str.find(",");
	if(ps<10000) {

	  e.m_elementName=str.substr(0,ps);
	  size_t ps2=str.find("{");
	  e.m_type=str.substr(ps+1,ps2-ps-1);

	}
	else {

	  e.m_elementName=str.substr(0,str.find("{")-1);
	}
	//e.m_elementName=str.substr(0,str.length()-2);
	e.m_depth=depth;
	elm.m_nodes.push_back(e);
	k++;



	while(std::getline(input,s)) {
	  if(s!="")
	    if(parseLine(s)==ELEMENT_START) {
	      depth++;
	      ConfigElement child=ConfigElement();
	      size_t ps=s.find(",");
	      if(ps<10000) {

		child.m_elementName=s.substr(0,ps);
		child.m_type=s.substr(ps+1,s.find("{")-ps-1);

	      }
	      else {

		child.m_elementName=s.substr(0,s.find("{")-1);
	      }
	      child.m_depth=depth;
	      elm.m_nodes.push_back(child);
	      k++;

	    }
	    else if(parseLine(s)==ATTRIBUTE) {
	      ConfigValue att=ConfigValue();
	      for(int i = 0; i < (int) s.length();i++)
		if(s[i]=='\"')
		  s[i]=' ';
	      size_t pos=s.find("=");

	      att.m_key=s.substr(0,pos);
	      att.m_value=s.substr(pos+1,s.length());

	      att.m_depth=depth;
	      elm.m_nodes[k-1].m_values.push_back(att);


	    }
	    else if(parseLine(s)==ELEMENT_END) {
	      depth--;
		      

	      if(depth==0) {
		k=0;

		for(int i=elm.m_nodes.size()-1;i>0;i--) {
		  if(elm.m_nodes[i].m_depth>elm.m_nodes[i-1].m_depth) {
		    elm.m_nodes[i-1].m_nodes.push_back(elm.m_nodes[i]);

		  }
		  else {

		    for(int j=i-2;j>=0;j--) {
		      if(elm.m_nodes[j].m_depth<elm.m_nodes[i].m_depth && (elm.m_nodes[i].m_depth-elm.m_nodes[j].m_depth)==1 ) {
			elm.m_nodes[j].m_nodes.push_back(elm.m_nodes[i]);
			break;
		      }


		    }
		  }


		}


		ConfigElement el= ConfigElement();
		el.m_nodes.push_back(elm.m_nodes[0]);
		m_doc.m_nodes.push_back(el);
		elm.m_nodes.clear();
		elm.m_values.clear();
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
    std::ofstream output(fileName);
    std::vector<std::string> ss;
    std::string aa=getConfigText(m_doc,ss);
    //	for(int i=ss.size()-1;i>=0;i--)
    //		output<<ss[i];
    output<<aa;
    output.close();


  }
  void ConfigDocument::TrimSpaces( std::string& str)  
  {  
    using namespace std;
    size_t startpos = str.find_first_not_of(" \t"); 
    size_t endpos = str.find_last_not_of(" \t"); 

    if(( std::string::npos == startpos ) || ( std::string::npos == endpos)) {  
      str = "";  
    }  
    else  
      str = str.substr( startpos, endpos-startpos+1 );  
 
  }
  ConfigElement *ConfigDocument::getConfigElement(std::string elementName)
  {
    bool found=false;
		
    ConfigElement *f=findConfigElement(m_doc,elementName,found);
    if(f)
      return f;
    else
      return 0;

  }
  ConfigElement *ConfigDocument::getConfigElement(std::string key,std::string value)
  {
    bool found=false;
		
    ConfigElement *f=findConfigElement(m_doc,found,key,value);
    if(f)
      return f;
    else
      return 0;

  }
  ConfigElement *ConfigDocument::findConfigElement(ConfigElement &e,bool &found,std::string key,std::string value)
  {
    for(int i=0;i < (int)e.m_nodes.size() ;i++)
      {
	ConfigElement *w;
	w=findConfigElement(e.m_nodes[i],found,key,value);
	if(found)
	  return w;

      }

    for(int j=0;j<(int)e.m_values.size();j++)
      {
	std::string ke=e.m_values[j].m_key;
	TrimSpaces(ke);
	std::string val=e.m_values[j].m_value;
	TrimSpaces(val);
	if(key==ke && value==val)
	  {
	    found=true;

	    return &e;

	  }
      }

    return 0;
  }

  ConfigElement *ConfigDocument::findConfigElement(ConfigElement &e,std::string elementName,bool &found)
  {
    for(int i=0;i < (int)e.m_nodes.size() ;i++)
      {
	ConfigElement *w;
	w=findConfigElement(e.m_nodes[i],elementName,found);
	if(found)
	  return w;

      }

    std::string s=e.m_elementName;
    TrimSpaces(s);
    if(s==elementName)
      {
	found=true;
		
	return &e;
			 
      }

    return 0;
  }
  std::string ConfigDocument::getConfigText(ConfigElement e,
					    std::vector<std::string> &s)
  {
    std::string str;

    //for(int i=e.m_nodes.size()-1;i>=0;i--)

    //	if(e.m_elementName!="")
    {
      if(e.m_elementName!="")
	{
	  if(e.m_type=="")
	    str+=e.m_elementName+" {";
	  else
	    str+=e.m_elementName+","+e.m_type+" {";
	  str+="\n";
	}

      for(int j=0;j < (int) e.m_values.size();j++)
	{
	  TrimSpaces(e.m_values[j].m_value);
	  str+=e.m_values[j].m_key+"="+"\""+e.m_values[j].m_value+"\""+"\n";

	}
      for(int i = 0; i < (int) e.m_nodes.size(); i++)
	str+=getConfigText(e.m_nodes[i],s);

      if(e.m_elementName!="")
	{
	  str+="}";
	  str+="\n";
	}
      //s.push_back(str);
    }
    return str;

  }
  ConfigDocument::ParseFlags ConfigDocument::parseLine(std::string line)
  {

    if(line[line.length()-1]=='{')
      return ELEMENT_START;
    else if(line[line.length()-1]=='}')
      return ELEMENT_END;
    else
      {
	size_t pos=line.find("=");
	if(pos>=10000)
	  return NOT_VALID;
	else
	  return ATTRIBUTE;
      }
  }

}
