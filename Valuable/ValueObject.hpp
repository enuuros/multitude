#ifndef VALUABLE_VALUE_OBJECT_HPP
#define VALUABLE_VALUE_OBJECT_HPP

#include <Radiant/BinaryData.hpp>

#include <Patterns/NotCopyable.hpp>

#include <CL/ClassLoader.hpp>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/util/XMLString.hpp>

#include <string>

namespace Valuable
{  
  class HasValues;

  class ValueObject : public Patterns::NotCopyable
  {
    public:
      ValueObject() : m_parent(0), m_transit(false) {}
      ValueObject(HasValues * parent, const std::string & name, bool transit);
      virtual ~ValueObject();
      
      std::string name() const { return m_name; }
      void setName(const std::string & s) { m_name = s; }
  
      std::string path() const;

      virtual void processMessage(const char * type, Radiant::BinaryData & data);

      virtual float       asFloat(bool * const ok = 0) const;
      virtual int         asInt(bool * const ok = 0) const;
      virtual std::string asString(bool * const ok = 0) const;
 
      /// Get the type id of the type 
      virtual const char * const type() const = 0;      

      virtual xercesc::DOMElement * serializeXML(xercesc::DOMDocument * doc);
      virtual bool deserializeXML(xercesc::DOMElement * element, CL::ClassLoader<ValueObject> & cl) = 0;

      HasValues * parent() { return m_parent; }
      void removeParent();

    protected:
      virtual void emitChange();
      virtual void emitDelete();

      HasValues * m_parent;
      std::string m_name;
      bool m_transit;

      friend class HasValues;
  };

}

#endif
