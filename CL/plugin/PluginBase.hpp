#ifndef CLASSLOADER_PLUGIN_BASE_HPP
#define CLASSLOADER_PLUGIN_BASE_HPP

class PluginBase
{
public:
  virtual ~PluginBase() {}
  virtual void foo() const;

  virtual void bar() = 0;
};

#endif
