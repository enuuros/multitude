/* COPYRIGHT
 *
 * This file is part of Luminous.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "Luminous.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include <Luminous/PixelFormat.hpp>

namespace Luminous
{
/*
  PixelFormat::PixelFormat() 
  : m_layout(LAYOUT_UNKNOWN),
    m_type(TYPE_UNKNOWN)
  {}
*/
  PixelFormat::PixelFormat(const PixelFormat& pf) 
  {
    m_layout = pf.m_layout;
    m_type = pf.m_type;
  }

  PixelFormat::PixelFormat(ChannelLayout layout, ChannelType type):
    m_layout(layout), 
    m_type(type)
  {}

  PixelFormat::~PixelFormat()
  {}

  int PixelFormat::numChannels() const 
  {
    switch(m_layout) {
      case LAYOUT_COLOR_INDEX:
      case LAYOUT_STENCIL_INDEX:
      case LAYOUT_DEPTH_COMPONENT:
      case LAYOUT_RED:
      case LAYOUT_GREEN:
      case LAYOUT_BLUE:
      case LAYOUT_ALPHA:
      case LAYOUT_LUMINANCE:
        return 1;
        break;
      case LAYOUT_LUMINANCE_ALPHA:
        return 2;
        break;
      case LAYOUT_RGB:
      case LAYOUT_BGR:
        return 3;
        break;
      case LAYOUT_RGBA:
      case LAYOUT_BGRA:
        return 4;
        break;          
      default:
        return 0;
    }
  }

  int PixelFormat::bytesPerPixel() const
  {
    int nc = numChannels();

    switch(m_type) {
      case TYPE_BYTE:
      case TYPE_UBYTE:
        return 1 * nc;
        break;
      case TYPE_SHORT:
      case TYPE_USHORT:
        return 2 * nc;
        break;
      case TYPE_INT:
      case TYPE_UINT:
      case TYPE_FLOAT:
        return 4 * nc;
        break;
      case TYPE_DOUBLE:
        return 8 * nc;
        break;
      default:
        return 0;
    }
  }

}
