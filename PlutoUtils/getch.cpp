#ifdef WIN32
#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

int getch()
{
	struct termios ts;
	struct termios new_ts;
	char c;
	ioctl(0, TCGETS, &ts);
	new_ts = ts;
	new_ts.c_lflag &= !ICANON;
	new_ts.c_lflag &= !ECHO;
	ioctl(0, TCSETS, &new_ts);

	c=getchar();

	ioctl(0, TCSETS, &ts);

	return c;
}
#endif
