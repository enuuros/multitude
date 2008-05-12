#ifndef CLASSLOADER_PLUGIN_SUB_HPP
#define CLASSLOADER_PLUGIN_SUB_HPP

#include "PluginBase.hpp"

class PluginSub : public PluginBase
{
public:
  void foo() const;
  void bar() {}
};

#endif
