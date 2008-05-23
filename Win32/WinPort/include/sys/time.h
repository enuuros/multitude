//
// torso
//
#ifndef _systime_h_
#define _systime_h_

#include <time.h>			// msvc

#ifdef __cplusplus
extern "C" {
#endif

struct timeval
{
	time_t	tv_sec;		// seconds
	long		tv_usec;		// microseconds
};

int nanosleep(const struct timespec *req, struct timespec *rem);
int gettimeofday(struct timeval *tp, void *tzp);

#ifdef __cplusplus
}
#endif

#endif