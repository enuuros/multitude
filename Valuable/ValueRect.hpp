#ifndef VALUABLE_VALUE_RECT_HPP
#define VALUABLE_VALUE_RECT_HPP

#include <Valuable/ValueObject.hpp>

#include <Nimble/Rect.hpp>

#define STD_OP this->emitChange(); return *this;

namespace Valuable
{

  class ValueRect : public ValueObject
  {
    public:
      ValueRect(HasValues * parent, const std::string & name, const Nimble::Rect & r, bool transit = false);

      ValueRect & operator = (const Nimble::Rect & r) { m_rect = r; STD_OP }

      const char * const type() const { return "rect"; }

      std::string asString(bool * const ok = 0) const;

      bool deserializeXML(DOMElement element);

      Nimble::Rect asRect() const { return m_rect; }

      //virtual bool set(const Nimble::Vector4f & v);

    protected:
      Nimble::Rect m_rect;
  };

}

#undef STD_OP

#endif
