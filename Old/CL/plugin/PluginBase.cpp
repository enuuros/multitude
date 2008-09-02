#include "PluginBase.hpp"

#include <iostream>

void PluginBase::foo() const 
{
  std::cout << "PluginBase::foo #" << std::endl;
}
