#ifndef CLASSLOADER_INSTANTIATOR_HPP
#define CLASSLOADER_INSTANTIATOR_HPP

#include "Factory.hpp"

#include <Radiant/Trace.hpp>

#include <string>
#include <map>

namespace CL
{

  template<typename BaseType, typename KeyType = std::string>
    class ClassLoader
    {
      public:
        typedef BaseType * (*FactoryType) ();
        typedef std::map<KeyType, FactoryType> ObjectFactoryMap;

        BaseType * instantiate(const KeyType & key)
        {
          typename ObjectFactoryMap::const_iterator it = m_factories.find(key);

          if(it != m_factories.end()) {
            return (it->second)();
          }

          Radiant::error("ClassLoader::instantiate # key '%s' is not registered", key.c_str());
          return 0;
        }

        void registerFactory(const KeyType & key, FactoryType fp = 0)
        {
          if(!fp) 
            fp = ObjectFactory<BaseType>::newInstance;

          m_factories.insert(typename ObjectFactoryMap::value_type(key, fp));
        }

        template<typename SubType>
          void registerSubType(const KeyType & key, FactoryType fp = 0)
          {
            if(!fp)
              fp = ObjectFactory<BaseType, SubType>::newInstance;

            registerFactory(key, fp);
          }

        bool isRegistered(const KeyType & key)
        {
          return m_factories.end() != m_factories.find(key);
        }

      private:
        ObjectFactoryMap m_factories;
    };

}

#endif
