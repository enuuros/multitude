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

#ifndef VALUABLE_HASVALUES_HPP
#define VALUABLE_HASVALUES_HPP

#include <Valuable/Export.hpp>
#include <Valuable/ValueObject.hpp>

#include <Radiant/Color.hpp>

#include <map>
#include <string>

//#include <xercesc/dom/DOM.hpp>

#define VO_TYPE_HASVALUES "HasValues"

namespace Valuable
{

  class VALUABLE_API HasValues : public ValueObject
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

      bool saveToFileXML(const char * filename);
      bool saveToMemoryXML(std::vector<char> & buffer);

      bool loadFromFileXML(const char * filename);

      virtual const char * const type() const { return VO_TYPE_HASVALUES; }

      DOMElement serializeXML(DOMDocument * doc);
      bool deserializeXML(DOMElement element);

      virtual bool readElement(DOMElement element);

      void debugDump();

      typedef std::map<std::string, ValueObject *> container;
      typedef container::iterator iterator;

      iterator valuesBegin() { return m_children.begin(); }
      iterator valuesEnd() { return m_children.end(); }

    protected: 
      container m_children;
  };

  /*
#ifdef WIN32
#ifdef VALUABLE_EXPORT
  // In WIN32 template functions must be instantiated to be exported
  template VALUABLE_API bool HasValues::setValue<float>(const std::string & name, const float &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector2T<float>>(const std::string & name, const Nimble::Vector2T<float> &);
  template VALUABLE_API bool HasValues::setValue<Nimble::Vector4T<float>>(const std::string & name, const Nimble::Vector4T<float> &);
#endif
#endif
  */
}

#endif
