#ifdef WIN32
#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

// these two could be merged into a generic_getch(bool echo) called by two inline wrappers called getch/getche

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

int getche()
{
	struct termios ts;
	struct termios new_ts;
	char c;
	ioctl(0, TCGETS, &ts);
	new_ts = ts;
	new_ts.c_lflag &= !ICANON;
	ioctl(0, TCSETS, &new_ts);

	c=getchar();

	ioctl(0, TCSETS, &ts);

	return c;
}
#endif
