#include "Size2D.hpp"
#include "Trace.hpp"

namespace Radiant
{

  Nimble::Vector2i resize(const Nimble::Vector2i & oldSize, const Nimble::Vector2i & newSize, bool keepAspect)
  {
    //trace("Radiant::resize(%d,%d, %d,%d, %s", oldSize.x, oldSize.y, newSize.x, newSize.y, keepAspect ? "yes" : "no");

    if(!keepAspect) {
      return newSize;
    } else {
      int rw = newSize.y * oldSize.x / oldSize.y;

      bool useHeight = (rw <= newSize.x);

      if(useHeight) {
        return Nimble::Vector2i(rw, newSize.y);
      } else {
        int y = newSize.x * oldSize.y / oldSize.x;
        int x = newSize.x;
        return Nimble::Vector2i(x, y);
      }
    }
  }

}
