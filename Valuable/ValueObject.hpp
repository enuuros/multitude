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

#ifndef VALUABLE_VALUE_OBJECT_HPP
#define VALUABLE_VALUE_OBJECT_HPP

#include <Nimble/Vector4.hpp>

#include <Patterns/NotCopyable.hpp>

#include <Radiant/BinaryData.hpp>

#include <Valuable/Export.hpp>
#include <Valuable/ValueListener.hpp>

#include <string>

namespace Valuable
{  
  class HasValues;
  class DOMElement;
  class DOMDocument;


  /// Base class for values 
  /** Typical child classes include some POD elements (floats, ints,
      vector2) etc, that can be accesses through the API.

      It is also possible to add listeners to values, so that if a
      value is changed, then a call-back to soem other object is
      followed. The listener-API is a bit hard-core, but it has plenty
      of power when you need to track the state of other objects.
  */
  class VALUABLE_API ValueObject
  {
    public:
      ValueObject();
      /// The copy constructor creates a copy of the ValueObject WITHOUT the
      /// link to parent
      ValueObject(const ValueObject & o);
      ValueObject(HasValues * parent, const std::string & name, bool transit = false);
      virtual ~ValueObject();
      
      const std::string & name() const { return m_name; }
      void setName(const std::string & s) { m_name = s; }
  
      std::string path() const;

      virtual void processMessage(const char * type, Radiant::BinaryData & data);

      virtual float       asFloat(bool * const ok = 0) const;
      virtual int         asInt(bool * const ok = 0) const;
      virtual std::string asString(bool * const ok = 0) const;

      virtual bool set(float v);
      virtual bool set(int v);
      virtual bool set(const std::string & v);
      virtual bool set(const Nimble::Vector2f & v);
      virtual bool set(const Nimble::Vector4f & v);
 
      /// Get the type id of the type 
      virtual const char * const type() const = 0;      

      virtual DOMElement serializeXML(DOMDocument * doc);
      virtual bool deserializeXML(DOMElement element) = 0;

      HasValues * parent() { return m_parent; }
      void removeParent();

      /// Adds a listener that is invoked whenever the value is changed
      void addListener(ValueListener * l) { m_listeners.push_back(l); }
      /// Removes a listener from the listener list
      void removeListener(ValueListener * l) { m_listeners.remove(l); }
    
    protected:
      /// Invokes the change valueChanged function of all listeners
      virtual void emitChange();
      /// Invokes the change valueDeleted function of all listeners
      virtual void emitDelete();
    private:
      /// The object that holds this object
      HasValues * m_parent;
      std::string m_name;
      bool m_transit;

      ValueListeners m_listeners;

      friend class HasValues;
  };

}

#endif
