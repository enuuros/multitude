#include "ClassLoader.hpp"
#include "plugin/PluginBase.hpp"

#include <Radiant/PlatformUtils.hpp>

#include <dlfcn.h>
#include <iostream>

class LocalType : public PluginBase
{
public:
  void foo() const { std::cout << "LocalType::foo #" << std::endl; }
};

int main(int , char ** )
{
  PluginBase * plugin = 0;

  std::string plugName("sub");
  std::string dllName("plugin/libPlugin.so");
  std::string regSymbol("registerClasses");

  CL::ClassLoader<PluginBase> loader;

  // Register a type manually
  loader.registerSubType<LocalType>("local");
  plugin = loader.instantiate("local");  
  plugin->foo();
  delete plugin;

  // Load the plugin
  void * lib = Radiant::PlatformUtils::openPlugin(dllName.c_str());
  if(!lib) {
    std::cerr << "Failed to load plugin '" << dllName << "'" << std::endl;
    return 1;
  }

  // Register the plugin classes
  typedef void (*reg_t)(CL::ClassLoader<PluginBase> & );
  reg_t reg = (reg_t)dlsym(lib, regSymbol.c_str());
  if(!reg) {
    std::cerr << "Failed to load symbol '" << regSymbol << "'" << std::endl;
    return 1;
  }

  // Call the register func
  reg(loader);

  // Instantiate from the plugin  
  plugin = loader.instantiate("base");  
  plugin->foo();
  delete plugin;

  plugin = loader.instantiate("sub");  
  plugin->foo();
  delete plugin;  

  return 0;
}

