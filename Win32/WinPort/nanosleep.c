#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#define _WINSOCKAPI_		// avoid redefining struct timeval
#include <windows.h>
#undef _WINSOCKAPI_			// avoid redefining struct timeval

// ----------------------------------------------------------------------------
// KLUDGE
// Windows Sleep() has millisecond's resolution, so we won't get microsecs or
// nanosecs here. Should use WaitForMultipleObjects() anyway...
// 'rem' is not updated
//
int nanosleep(const struct timespec * req, struct timespec * rem)
{
	if (req)
	{
		uint32_t dur = (uint32_t)(req->tv_sec * 1000 + req->tv_nsec / 1000000);
		Sleep(dur);
		return 0;
	}
	return -1;
}
