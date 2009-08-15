/* COPYRIGHT
 *
 * This file is part of Valuable.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Valuable.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUABLE_CONFIG_DOCUMENT_HPP
#define VALUABLE_CONFIG_DOCUMENT_HPP

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
    enum ParseFlags
    {
      ELEMENT_START,
      ELEMENT_END,
      ATTRIBUTE,
      NOT_VALID

    };
    ParseFlags parseLine(string line);
  };
}

#endif
