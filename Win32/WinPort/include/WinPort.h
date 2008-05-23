#ifndef _WinPort_h_
#define _WinPort_h_

#include <stdint.h>

typedef int				socklen_t;
typedef unsigned		uint;
typedef uint64_t		u_int64_t;
typedef uint64_t		u_int64_t;
typedef unsigned char	uint8_t;

#define strtoll(n,e,b)		_strtoi64(n,e,b)
#define strtof(f1,f2)		((float)strtod(f1,f2))
#define bzero(b,len)		(memset((b), '\0', (len)), (void)0)
#define EHOSTUNREACH		WSAEHOSTUNREACH
#define SIGKILL				SIGTERM

#define READFDS	1
#define WRITEFDS	2
#define EXCEPTFDS	4

extern "C" {
int sleep(unsigned secs);
long round(double x);
int setenv(const char *envname, const char *envval, int overwrite_ignored);
int unsetenv(const char *name);
}

namespace WinPort
{
	int  initSockets();
	void exitSockets();
}

#endif
