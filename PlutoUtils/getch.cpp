#ifdef WIN32
#include <conio.h>
#else
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <iostream>
using namespace std;

int THE_getch(bool echo) // :P
{
	struct termios ts;
	struct termios new_ts;
	char c;
	ioctl(0, TCGETS, &ts);
	new_ts = ts;
	new_ts.c_lflag &= !ICANON;
	if (! echo)
		new_ts.c_lflag &= !ECHO;
	ioctl(0, TCSETS, &new_ts);

	cin >> c;

	ioctl(0, TCSETS, &ts);

	return c;
}

int getch()
{
	return THE_getch(false);
}

int getche()
{
	return THE_getch(true);
}
#endif
