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

#ifndef VALUEIO_HAS_VALUES_HPP
#define VALUEIO_HAS_VALUES_HPP

#include <ValueIO/IO.hpp>

#include <Nimble/Vector2.hpp>
#include <Nimble/Vector3.hpp>
#include <Nimble/Vector4.hpp>

#include <map>
#include <vector>

#ifdef WIN32
#include <WinPort.h>
#pragma warning( disable: 4251 )
// see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
#endif

namespace ValueIO {

  class Value;

  /// Base class for automatic value IO and parameter access
  /** By inheriting this class you can read/write member variables to
      XML files and allow them to be accessed based on their name. 
      
      The general idea is that a class can store pointers to its
      member variables into a list. One can then do automatic XML IO
      and access the member variables.

      As this class stores pointers to the actual member variables,
      one should not copy objects that inherit his class.

      The typical use pattern of this class is to inherit this class
      and store information on the member variables in the
      constructor:

      <PRE>
      class MyClass : public ValueIO::HasValues
      {
      public:
        MyClass()
	  : m_mode(1), m_weight(100.0f)
        {
	  addValue("mode",   m_mode);
	  addValue("weight", m_weight);

	  Now m_mode and m_weight are available to XML serialization,
	  and also accessible using the "value" methods.
	}
      protected:
        int   m_mode;
	float m_weight;
      };
      </PRE>

      Typical ways to use the class are then:

      <PRE>
      MyClass obj;

      Read and adjust a value:

      HasValues::Ptr valptr = obj.value(name);
      float newweight = valptr.getFloat() * 1.5f;
      valptr.setFloat(newweight);

      Save to a file:

      obj.save("example.xml");
      </PRE>
  */
  class EXPORT HasValues
  {    
  public:

    /// Holds a (void) pointer to a member variable
    /** Together with the void pointer we store a pointer to an IO
	object that can handle the particular type of objects. 

	<B>Note:</B> For the sake of preformance, the various get/set
	functions do not have safety checks. To check that the Ptr
	object really points somewhere, you can use the isNull()
	method.
    */
    class Ptr
    {
      public:
        Ptr(void * ptr = 0, IO * io = 0)
          : m_ptr(ptr), m_io(io) {}
        void * m_ptr;
        IO * m_io;

      /// Returns true if the internal pointer is null
      inline bool isNull() const { return m_io == 0; }

      /// Converts the pointer to a float
      float getFloat() { return m_io->getFloat(m_ptr, 0); }
      float getFloat(int index) { return m_io->getFloat(m_ptr, index); }
      /// Converts the pointer to a 2D vector
      Nimble::Vector2 getVector2() { return m_io->getVector2(m_ptr); }
      /// Converts the pointer to a 4D vector
      Nimble::Vector4 getVector4() { return m_io->getVector4(m_ptr); }

      /// Ses the pointer value to float v
      void  setFloat(float v) { m_io->setFloat(m_ptr, 0, v); }
      void  setFloat(int index, float v) { m_io->setFloat(m_ptr, index, v); }
      /// Ses the pointer value to 2D vector v
      void  setVector2(const Nimble::Vector2f & v) { m_io->setVector2(m_ptr,v);}
      /// Ses the pointer value to 4D vector v
      void  setVector4(const Nimble::Vector4f & v) { m_io->setVector4(m_ptr,v);}

      void setString(const std::string & str) { m_io->setString(m_ptr, str); }
    };

    typedef std::map<std::string, Ptr> container;
    typedef container::iterator iterator;

    HasValues();
    virtual ~HasValues();

    /// The name (ususally type) of this object
    const std::string & name() const { return m_name; }
    /// Set the name (ususally type) of this object
    void setName(const std::string & name);
    void setName(const char * name);

    void forgetValue(const std::string & id);
    /// Get a pointer to a particular parameter
    Ptr value(const std::string & id);
    /// Get a pointer to a particular parameter
    Ptr value(const XMLCh * id);

    /// Writes the member variables to an XML DOM element
    /** This method is virtual, so that child classes can implement
	more logic on top of the basic, automatical XML processing. */
    virtual DOMElement * writeDom(DOMDocument *);
    /// Reads the member variables from an XML DOM element
    virtual bool readDom(DOMElement *);

    /// Iterator to the first value pointer
    iterator valuesBegin() { return m_values.begin(); }
    /// End iterator to the final (one past the end) value pointer
    iterator valuesEnd()   { return m_values.end(); }

    /// Loads this object from a file.
    bool load(const char * filename);
    /// Saves this object to a file.
    bool save(const char * filename);

    /// Perform XML IO initializations 
    /** This function can be called many times, but only the first
	call will have any affect. */
    static void init();
    /// Run a simple test, creating and reading a new XML file.
    static bool test(const char * filename);

  private:
    /// Disabled, as this object cannot be copied around
    HasValues(const HasValues &);
  
  protected:
    void addValue(const char * id, std::string * v)
    { addValueInternal(id, v, & IOStdString::instance); }
    void addValue(const char * id, std::wstring * v)
    { addValueInternal(id, v, & IOStdWString::instance); }
    void addValue(const char * id, HasValues * v)
    { addValueInternal(id, v, & IOHasValues::instance); }
    void addValue(const char * id, int32_t & v)
    { addValueInternal(id, &v, & IOInt32::instance); }
    void addValue(const char * id, int64_t & v)
    { addValueInternal(id, &v, & IOInt64::instance); }
    void addValue(const char * id, float & v)
    { addValueInternal(id, &v, & IOFloat32::instance); }
    void addValue(const char * id, double & v)
    { addValueInternal(id, &v, & IOFloat64::instance); }
    void addValue(const char * id, Nimble::Vector2 & v)
    { addValueInternal(id, &v, & IOVectorFloat32::instance2); }

    void addValue(const char * id, Nimble::Vector3 & v)
    { addValueInternal(id, &v, & IOVectorFloat32::instance3); }
    void addValue(const char * id, Nimble::Vector4 & v)
    { addValueInternal(id, &v, & IOVectorFloat32::instance4); }
    void addValue(const char * id, Nimble::Vector2i & v)
    { addValueInternal(id, &v, & IOVectorInt32::instance2); }
#ifndef WIN32	// JJK: IOVectorUInt8::instance4 not found
    void addValue(const char * id, Nimble::Vector4ub& v)
    { addValueInternal(id, &v, & IOVectorUInt8::instance4); }
#endif
    void addValue(const char * id, std::vector<Nimble::Vector2> & v)
    { addValueInternal(id, &v, & IOStdVectorVectorFloat32::instance2); }
    
    /// Clears all value pointers from this object.
    void clearValues();

    virtual bool readValue(DOMElement *);
    
    void addValueInternal(const char * id, void * ptr, IO * io);
    
  private:
    container m_values;

    std::string m_name;
  };


};

#endif

