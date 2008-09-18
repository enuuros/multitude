#include <Nimble/Rect.hpp>

namespace Nimble
{

  template <class T> 
    bool RectT<T>::contains(Vector2T<T> v) const
    {
      return ((v[0] >= m_low[0]) && (v[0] <= m_high[0]) && (v[1] >= m_low[1]) && (v[1] <= m_high[1]));  
    }


}

