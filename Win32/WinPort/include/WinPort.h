#ifndef _WinPort_h_
#define _WinPort_h_

#include <WinPort/Export.hpp>

#include <stdint.h>

typedef int				  socklen_t;
typedef unsigned		uint;
typedef uint64_t		u_int64_t;
typedef uint64_t		u_int64_t;
typedef unsigned char	uint8_t;

#define atoll(s)        _atoi64(s)
#define strtoll(n,e,b)	_strtoi64(n,e,b)
#define strtof(f1,f2)		((float)strtod(f1,f2))
#define bzero(b,len)		(memset((b), '\0', (len)), (void)0)
#define EHOSTUNREACH		WSAEHOSTUNREACH
#define SIGKILL				  SIGTERM

#define READFDS	  1
#define WRITEFDS	2
#define EXCEPTFDS	4

extern "C" WINPORT_API int sleep(unsigned secs);
extern "C" WINPORT_API long round(double x);
extern "C" WINPORT_API int setenv(const char * envname, const char * envval, int overwrite_ignored);
extern "C" WINPORT_API int unsetenv(const char * name);

namespace WinPort
{
	WINPORT_API int  initSockets();
	WINPORT_API void exitSockets();

  // This doesn't do anything, just ensures that .lib file is built
  WINPORT_API int tmp();
}

#endif
