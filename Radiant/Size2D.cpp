#include "Size2D.hpp"

namespace Radiant
{

  Nimble::Vector2i resize(Nimble::Vector2i oldSize, Nimble::Vector2i newSize, bool keepAspect)
  {
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

  Nimble::Vector2i fitToSize(float aspect, Nimble::Vector2i constraint)
  {
    int rw = static_cast<int> (constraint.y * aspect);
    bool useHeight = (rw <= constraint.x);

    if(useHeight) 
      return Nimble::Vector2i(rw, constraint.y);
    else 
      return Nimble::Vector2i(constraint.x, static_cast<int> (constraint.x / aspect));
  }

}
