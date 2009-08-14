#include <Valuable/Export.hpp>
#include <string>
namespace Valuable
{  
using namespace std;

class VALUABLE_API Attribute
{
public:
	Attribute(void);
	Attribute(string k,string v);
	virtual ~Attribute(void);
	int depth;
	string key,value;
};
}