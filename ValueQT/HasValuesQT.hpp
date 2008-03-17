/* COPYRIGHT
 *
 * This file is part of ValueQT.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ValueQT.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */


#ifndef VALUEQT_HAS_VALUES_QT_HPP
#define VALUEQT_HAS_VALUES_QT_HPP

#include <ValueIO/HasValues.hpp>

#include <QString>

/* We could use namespace ValueQT, but maybe it is easier to just say
   that this is part of the same package, but different library. */

namespace ValueIO {

  class IOQString : public IO
  {
  public:
    virtual ~IOQString() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOQString instance;
  };
  

  class HasValuesQT : public HasValues
  {
  public:
    HasValuesQT();
    ~HasValuesQT();

  protected:
    void addValueQT(const char * id, QString & v)
    { addValueInternal(id, & v, & IOQString::instance); }

    
  };
  
}

#endif

