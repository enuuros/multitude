#ifndef VALUABLE_HASVALUES_HPP
#define VALUABLE_HASVALUES_HPP

#include <Valuable/ValueObject.hpp>

#include <Radiant/Color.hpp>

#include <map>
#include <string>

#include <xercesc/dom/DOM.hpp>

#define VO_TYPE_HASVALUES "HasValues"

namespace Valuable
{

  class HasValues : public ValueObject
  {
    public:
      HasValues();
      HasValues(HasValues * parent, const std::string & name, bool transit = false);
      virtual ~HasValues();

      bool addValue(const std::string & name, ValueObject * const value);
      ValueObject * getValue(const std::string & name);
      void removeValue(ValueObject * const value);      

      /// @todo add 'shortcut' API
      // float getValueFloat(const std::string & name, bool * ok = 0, float default = 0.f)
      // ...
      
      template<class T>
      bool setValue(const std::string & name, const T & v);

      bool saveXML(xercesc::XMLFormatTarget & target);
      bool saveXML(const char * filename);
      bool saveInMemoryXML(std::vector<char> & buffer);
      bool loadXML(const char * filename);

      virtual const char * const type() const { return VO_TYPE_HASVALUES; }

      xercesc::DOMElement * serializeXML(xercesc::DOMDocument * doc);
      bool deserializeXML(xercesc::DOMElement * element);

      virtual bool readElement(xercesc::DOMElement * element);

      void debugDump() ;

      typedef std::map<std::string, ValueObject *> container;
      typedef container::iterator iterator;

      iterator valuesBegin() { return m_children.begin(); }
      iterator valuesEnd() { return m_children.end(); }

    protected: 
      container m_children;
  };

}

#endif
