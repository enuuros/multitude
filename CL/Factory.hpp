#ifndef CLASSLOADER_FACTORY_HPP
#define CLASSLOADER_FACTORY_HPP

namespace CL
{

  template<typename BaseType, typename SubType=BaseType>
    struct ObjectFactory
    {
      static BaseType * newInstance() { return new SubType; }
      static BaseType * noInstance() { return 0; }
    };

}

#endif
