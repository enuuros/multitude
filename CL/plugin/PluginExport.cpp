#include <CL/ClassLoader.hpp>

#include "PluginBase.hpp"
#include "PluginSub.hpp"

extern "C" {

  void registerClasses(CL::ClassLoader<PluginBase> & loader)
  {
    loader.registerFactory("base");
    loader.registerSubType<PluginSub>("sub");
  }

}
