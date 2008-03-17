#include <ValueIO/HasValues.hpp>
#include <iostream>

using namespace ValueIO;
using namespace std;

class Foo : public HasValues
{
public: 
  Foo() 
  : HasValues(), 
  m_f1(-1.f),
  m_i1(-1)
  {
    addValue("floatti", m_f1);
    addValue("intti", m_i1);
  }  

  float m_f1;
  int m_i1;
};

int main(int , char **)
{
  Foo foo;

  float a = foo.value("floatti").getFloat();
  cout << "'f1' equals " << a << endl;  
  foo.value("floatti").setFloat(1.f);
  cout << "'f1' equals " << foo.value("floatti").getFloat() << endl;

  float b = foo.value("intti").getFloat();
  cout << "'i1' equals " << b << endl;
  foo.value("intti").setFloat(4.5f);
  cout << "'i1' equals " << foo.value("intti").getFloat() << endl;
  
  return 0;
}
