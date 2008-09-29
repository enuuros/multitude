#include <Valuable/HasValues.hpp>
#include <Valuable/Valuable.hpp>
#include <Valuable/ValueFloat.hpp>
#include <Valuable/ValueRect.hpp>
#include <Valuable/ValueString.hpp>

using namespace Valuable;

int main(int, char **)
{
  Valuable::initialize();

  HasValues hv(0, "apina");

  ValueFloat v(&hv, "kissa", 1.f);
  ValueRect r(&hv, "nelio", Nimble::Rect(0.f, 0.f, 1.f, 1.f));
  ValueWString ws(&hv, "unicode", L"mååmömi");

  bool res = hv.saveToFileXML("test.xml");

  printf("save %s.\n", res ? "ok" : "fail");

  res = hv.loadFromFileXML("test.xml");

  printf("load %s.\n", res ? "ok" : "fail");

  Valuable::terminate();

  return 0;
}
