#include "WinPort.h"
#include <math.h>
#include <Windows.h>

long round(double x)
{
	return (long)floor(x + 0.5);
}

int setenv(const char *envname, const char *envval, int overwrite_ignored)
{
	return SetEnvironmentVariable(envname, envval);
}

int unsetenv(const char *name)
{
	return SetEnvironmentVariable(name, NULL);
}

namespace WinPort
{
	int initSockets()
	{
		WORD wVersion = MAKEWORD(2,2);
		WSADATA wsaData;
		return WSAStartup(wVersion, &wsaData);
	}

	void exitSockets()
	{
		WSACleanup();
	}
}
