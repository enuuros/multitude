/* COPYRIGHT
 *
 * This file is part of ValueIO.
 *
 * Copyright: Helsinki University of Technology, MultiTouch Oy and others.
 *
 * See file "ValueIO.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#ifndef VALUEIO_IO_HPP
#define VALUEIO_IO_HPP

#include <xercesc/dom/DOM.hpp>
#include <Radiant/Trace.hpp>
#include <Nimble/Vector4.hpp>

namespace ValueIO {

  XERCES_CPP_NAMESPACE_USE;

  class HasValues;

  /// Base class for automatic IO and parameter adjustment.
  /** The functionality of this class is diveded into two parts -
      automated parameters IO and value access.

      1) The automatic IO is done by saving the given parameter to an XML
      file. 

      2) Parameter access. This enables one to access arbitrary
      parameters in C++, so that you can for example access objects
      member variables based on their names. IO objects are used to
      convert the paramters so that you can access them as floats or
      vectors as needed.
  */
  class IO
  {
  public:
    virtual ~IO() {}
    /// Read a value from a Xerces DOMElement
    /** @param ptr Pointer to the value to read
	@param host The host object that owns the pointer
	@param n The XML DOM node
	@param e The XML DOM element

	@return This function returns true on success, and false on
	failure.
     */
    virtual bool readDom(void * ptr, HasValues * host,
			 DOMNode * n, DOMElement * e) = 0;
    /// Write the value to a Xerces DOMNode
    /** @param ptr Pointer to the value to write
	@param host The host object that owns the pointer
	@param doc The XML documen that the node will go into

	@return The function returns an XML node that contains the
	value of the object
     */
    virtual DOMNode * writeDom(void * ptr, HasValues * host,
			       DOMDocument * doc) = 0;
  
    /// Try to get a floating point value from the object pointer
    virtual float getFloat(void * ptr, int index);
    /// Try to set a floating point value to the object pointer
    virtual void  setFloat(void * ptr, int index, float v);

    virtual Nimble::Vector4 getVector4(void * ptr);
    virtual void  setVector4(void * ptr, const Nimble::Vector4 & v);

    virtual void setVector2(void * ptr, const Nimble::Vector2 & v);
    virtual Nimble::Vector2 getVector2(void * ptr);

    virtual void setString(void * ptr, const std::string & str);
  };
  
  class IOStdString : public IO
  {
  public:
    virtual ~IOStdString() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);

    virtual void setString(void * ptr, const std::string & str) { (*(std::string*)ptr) = str; }
    
    static IOStdString instance;
  };

  class IOStdWString : public IO
  {
  public:
    virtual ~IOStdWString() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOStdWString instance;
  };

  class IOHasValues : public IO
  {
  public:
    virtual ~IOHasValues() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOHasValues instance;
  };

  class IOInt32 : public IO
  {
  public:
    virtual ~IOInt32() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);

    virtual float getFloat(void * ptr, int index) { return (float)((int*)ptr)[index]; }
    virtual void setFloat(void * ptr, int index, float v) { ((int*)ptr)[index] = (int)v; }
    
    static IOInt32 instance;
  };

  class IOInt64 : public IO
  {
  public:
    virtual ~IOInt64() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOInt64 instance;
  };

  class IOFloat32 : public IO
  {
  public:
    virtual ~IOFloat32() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);

    virtual float getFloat(void * ptr, int index) { return ((float*)ptr)[index]; }
    virtual void  setFloat(void * ptr, int index, float v) { ((float*)ptr)[index] = v; }
    
    static IOFloat32 instance;
  };

  class IOFloat64 : public IO
  {
  public:
    virtual ~IOFloat64() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOFloat64 instance;
  };

  class IOVectorFloat32 : public IO
  {
  public:
    IOVectorFloat32(int size) : m_size(size) {}
    virtual ~IOVectorFloat32() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);

    virtual Nimble::Vector4 getVector4(void * ptr) { return Nimble::Vector4((float*)ptr); }
    virtual void setVector4(void * ptr, const Nimble::Vector4 & v) { *(Nimble::Vector4*)ptr = v; }
   
    virtual Nimble::Vector2 getVector2(void * ptr) { return Nimble::Vector2((float*)ptr); }
    virtual void setVector2(void * ptr, const Nimble::Vector2 & v) { *(Nimble::Vector2*)ptr = v; }
 
    static IOVectorFloat32 instance2;
    static IOVectorFloat32 instance3;
    static IOVectorFloat32 instance4;
  protected:
    int m_size;
  };

  class IOVectorInt32 : public IO
  {
  public:
    IOVectorInt32(int size) : m_size(size) {}
    virtual ~IOVectorInt32() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOVectorInt32 instance2;
    static IOVectorInt32 instance3;
    static IOVectorInt32 instance4;
  protected:
    int m_size;
  };

  class IOVectorUInt8 : public IO
  {
  public:
    IOVectorUInt8(int size) : m_size(size) {}
    virtual ~IOVectorUInt8() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOVectorUInt8 instance4;
  protected:
    int m_size;
  };

  class IOStdVectorVectorFloat32 : public IO
  {
  public:
    IOStdVectorVectorFloat32(int size) : m_size(size) {}
    virtual ~IOStdVectorVectorFloat32() {}
    virtual bool readDom(void * ptr, HasValues *, DOMNode * e, DOMElement *);
    virtual DOMNode * writeDom(void *, HasValues *, DOMDocument *);
    
    static IOVectorFloat32 instance2;
    static IOVectorFloat32 instance3;
    static IOVectorFloat32 instance4;
  protected:
    int m_size;
  };

}

#endif
