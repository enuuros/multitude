#include "include/WinPort.h"
#include <math.h>
#include <windows.h>

int sleep(unsigned secs)
{
	Sleep(secs*1000);
	return 0;
}

long round(double x)
{
	return (long)floor(x + 0.5);
}

int setenv(const char *envname, const char *envval, int /*overwrite_ignored*/)
{
	return SetEnvironmentVariableA(envname, envval);
}

int unsetenv(const char *name)
{
	return SetEnvironmentVariableA(name, NULL);
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

  int tmp() { return 0; }
}
