// misc.c

#include "stdio.h"
#include "globals.h"

#ifdef WIN32
#include "windows.h"
#include "time.h"
#include <iostream>
#include "fcntl.h"
//#include "stdio.h"
#else
#include "sys/time.h"
#include "sys/select.h"
#include "unistd.h"
#include "string.h"
#endif

//Returns current time in ms
int Get_Time_Ms(void)
{
	return clock() * 1000 / CLOCKS_PER_SEC;
}


// http://home.arcor.de/dreamlike/chess/
int InputWaiting()
{
#ifndef WIN32
	fd_set readfds;
	struct timeval tv;
	FD_ZERO(&readfds);
	FD_SET(fileno(stdin), &readfds);
	tv.tv_sec = 0; tv.tv_usec = 0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(fileno(stdin), &readfds));
#else
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;

	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
		}
	}
	if (pipe) {
		if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
		return dw;
	}
	else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
#endif
}

void ReadInput(SEARCH_INFO_STRUCT *info) {
	int             bytes;
	char            input[256] = "", *endc;

	if (InputWaiting()) {
		//info->stopped = TRUE;
		do {
			//bytes = read(_fileno(stdin), input, 256);
			bytes = (int)fgets(input, 256, stdin);
		} while (bytes<0);
		endc = strchr(input, '\n');
		if (endc) *endc = 0;

		if (strlen(input) > 0) {
			if (!strncmp(input, "quit", 4))    {
				info->quit = TRUE;
				info->stopped = TRUE;
			}
			else if (!strncmp(input, "stop", 4))    {
				info->stopped = TRUE;
			}
		}
		return;
	}
}