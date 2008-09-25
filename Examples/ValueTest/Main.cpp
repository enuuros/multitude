#include <Valuable/HasValues.hpp>
#include <Valuable/ValueFloat.hpp>
#include <Valuable/Valuable.hpp>

using namespace Valuable;

int main(int, char **)
{
  Valuable::initialize();

  HasValues hv(0, "apina");

  ValueFloat v(&hv, "kissa", 1.f);

  bool r = hv.saveToFileXML("test.xml");

  printf("save %s.\n", r ? "ok" : "fail");

  r = hv.loadFromFileXML("test.xml");

  printf("load %s.\n", r ? "ok" : "fail");

  Valuable::terminate();

  return 0;
}
