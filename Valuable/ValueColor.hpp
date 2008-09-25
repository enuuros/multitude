#ifndef VALUABLE_VALUE_COLOR_HPP
#define VALUABLE_VALUE_COLOR_HPP

#include <Valuable/ValueObject.hpp>

#include <Radiant/Color.hpp>

#define STD_OP this->emitChange(); return *this;

namespace Valuable
{

  class ValueColor : public ValueObject
  {
    public:
      ValueColor(HasValues * parent, const std::string & name, const Radiant::Color & c, bool transit = false);

      ValueColor & operator = (const Radiant::Color & c) { m_color = c; STD_OP }

      const char * const type() const { return "color"; }

      std::string asString(bool * const ok = 0) const;

      bool deserializeXML(DOMElement element);

      Radiant::Color asColor() const { return m_color; }

      virtual bool set(const Nimble::Vector4f & v);

    protected:
      Radiant::Color m_color;
  };

}

#undef STD_OP

#endif
