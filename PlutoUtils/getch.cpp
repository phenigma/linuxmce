/* 
	getch
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

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

	while ((c = getchar()) == '\r') {}
//	cin >> c;

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
