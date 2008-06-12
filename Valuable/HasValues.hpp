#ifndef VALUABLE_HASVALUES_HPP
#define VALUABLE_HASVALUES_HPP

#include <Valuable/ValueObject.hpp>

#include <CL/ClassLoader.hpp>

#include <map>
#include <string>

#include <xercesc/dom/DOM.hpp>

#define VO_TYPE_HASVALUES "HasValues"

namespace Valuable
{

  class HasValues : public ValueObject
  {
    public:
      HasValues() : ValueObject() {}
      HasValues(HasValues * parent, const std::string & name, bool transit);
      virtual ~HasValues();

      bool addValue(const std::string & name, ValueObject * const value);
      ValueObject * getValue(const std::string & name);
      void removeValue(ValueObject * const value);      

      /// @todo add 'shortcut' API
      // float getValueFloat(const std::string & name, bool * ok = 0, float default = 0.f)
      // ...

      bool saveXML(const char * filename);
      bool loadXML(const char * filename, CL::ClassLoader<ValueObject> & cl);

      virtual const char * const type() const { return VO_TYPE_HASVALUES; }

      xercesc::DOMElement * serializeXML(xercesc::DOMDocument * doc);
      bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & cl);

      virtual bool readElement(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & cl);

      void debugDump() ;

    protected: 

      typedef std::map<std::string, ValueObject *> container;

      container m_children;
  };

}

#endif
