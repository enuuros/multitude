/* COPYRIGHT
 *
 * This file is part of CL.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "CL.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */
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
