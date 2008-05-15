#include "Valuable.hpp"

#include <xercesc/util/PlatformUtils.hpp>

XERCES_CPP_NAMESPACE_USE;

namespace Valuable
{

  void initialize()
  {
    XMLPlatformUtils::Initialize();
  }

  void terminate()
  {
    XMLPlatformUtils::Terminate();
  }

}
