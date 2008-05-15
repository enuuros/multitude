#ifndef VALUABLE_CHANGEMAP_HPP
#define VALUABLE_CHANGEMAP_HPP

#include <set>

namespace Valuable
{

  class ValueObject;

  class ChangeMap 
  {
    public:
      ChangeMap();
      virtual ~ChangeMap();

      static void addDelete(ValueObject * vo);
      static void addChange(ValueObject * vo);

    protected:
      virtual void queueChange(ValueObject * vo);

      static ChangeMap * instance;

      typedef std::set<ValueObject *> container;
      container m_changes;
  };


}

#endif
