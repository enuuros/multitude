#include "PluginSub.hpp"

#include <iostream>

void PluginSub::foo() const 
{
  std::cout << "PluginSub::foo #" << std::endl;
}
