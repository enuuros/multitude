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
#include "ConfigElement.hpp"

// 

namespace Valuable
{  
  class VALUABLE_API ConfigDocument
  {
  public:
    ConfigDocument(void);
    virtual ~ConfigDocument(void);

    void readConfigFile(char *fileName);
    void writeConfigFile(char *fileName);
    ConfigElement *getConfigElement(std::string elementName);
    ConfigElement *getConfigElement(std::string key,std::string value);
  private:

    void loadConfigElement(std::string str);
    void TrimSpaces( std::string& str);
    void loadConfigValue(std::string key,std::string val);
    std::string getConfigText(ConfigElement e,std::vector<std::string> &ss);
    ConfigElement *findConfigElement(ConfigElement &e,std::string elementName,bool &found);
    ConfigElement *findConfigElement(ConfigElement &e,bool &found,std::string key,std::string value);
    enum ParseFlags
    {
      ELEMENT_START,
      ELEMENT_END,
      ATTRIBUTE,
      NOT_VALID

    };

    ParseFlags parseLine(std::string line);

    ConfigElement m_doc;		
  };
}

#endif
