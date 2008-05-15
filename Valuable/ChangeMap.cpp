#include "ChangeMap.hpp"

#include <Radiant/Trace.hpp>

namespace Valuable
{

  ChangeMap * ChangeMap::instance = 0;

  ChangeMap::ChangeMap()
  {
    if(instance) {
      Radiant::error("ChangeMap::ChangeMap # instance already exists, replacing it.");
    } 

    instance = this;
  }

  ChangeMap::~ChangeMap()
  {
    if(instance == this) instance = 0;
  }

  void ChangeMap::addDelete(ValueObject * )
  {
    
  }

  void ChangeMap::addChange(ValueObject * vo) 
  {
    if(instance) 
      instance->queueChange(vo);
  }

  void ChangeMap::queueChange(ValueObject * vo) 
  { 
    m_changes.insert(vo); 
  }

}
