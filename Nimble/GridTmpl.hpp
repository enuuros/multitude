/* COPYRIGHT
 *
 * This file is part of Nimble.
 *
 * Copyright: Nimble Oy, Finland, http://multitou.ch
 *
 * All rights reserved, 2007-2008
 *
 * You may use this file only for purposes for which you have a
 * specific, written permission from Nimble Oy.
 *
 * See file "Nimble.hpp" for authors and more details.
 *
 */

#ifndef NIMBLE_GRID_TMPL_HPP
#define NIMBLE_GRID_TMPL_HPP

#include "Grid.hpp"

namespace Nimble {

  template <class T>
  GridMemT<T>::GridMemT(unsigned w, unsigned h)
    : m_width(w), m_height(h)
  {
    unsigned s = w * h;
    if(s)
      m_data = new T[s];
    else
      m_data = 0;
  }

  template <class T>
  GridMemT<T>::~GridMemT()
  {
    delete [] m_data;
  }

  template <class T>
  void GridMemT<T>::resize(unsigned w, unsigned h)
  {
    unsigned s = w * h;
    unsigned smy = m_width * m_height;
    
    m_width = w;
    m_height = h;
    
    if(s == smy)
      return;
    
    delete [] m_data;
    
    if(s)
      m_data = new T[s];
    else
      m_data = 0;
  }
}

#endif
