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

#include "HasValuesQT.hpp"

#include <ValueIO/Dom.hpp>

namespace ValueIO {


  bool IOQString::readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *)
  {
    QString & val = * (QString *) ptr;
    DOMText * text = dynamic_cast<DOMText *> (e);

    if(!text) return false;

    const XMLCh * nv = text->getNodeValue();

    int len = nv ? XMLString::stringLen(nv) : 0;

    val.resize(len);

    for(int i = 0; i < len; i++)
      val[i] = nv[i];

    return true;
  }

  DOMNode * IOQString::writeDom(void * ptr, HasValues *, DOMDocument * doc)
  {
    const QString & val = * (QString *) ptr;
    std::vector<XMLCh> xml;
    int len =  val.size();

    if(len) {
      xml.resize(len + 1);
      for(int i = 0; i < len; i++)
	xml[i] = (XMLCh) val[i].unicode();
    }
    else
      xml.resize(1);

    xml[len] = 0;
    
    return doc->createTextNode(&xml[0]);
  }
  
  IOQString IOQString::instance;


  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////

  HasValuesQT::HasValuesQT()
  {}
  
  HasValuesQT::~HasValuesQT()
  {}


}
