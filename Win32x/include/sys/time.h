//
// torso
//
#ifndef _systime_h_
#define _systime_h_

#include <Win32x/include/Export.hpp>

#include <pthread.h>
#include <time.h>			// msvc
//#include <Winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IGNORE_MULTITUDE_TIMEVAL
struct timeval
{
	time_t	tv_sec;		// seconds
	long		tv_usec;		// microseconds
};
#endif

WINPORT_API int nanosleep(const struct timespec *req, struct timespec *rem);
WINPORT_API int gettimeofday(struct timeval *tp, void *tzp);

#ifdef __cplusplus
}
#endif

#endif
