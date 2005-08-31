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
#include <unistd.h>

#define EOF -1

#include <iostream>
using namespace std;

int THE_getch(bool echo,int iTimeoutInSeconds) // :P
{
	struct termios ts;
	struct termios new_ts;
	char c;
	
	fflush(stdout);
	
	ioctl(0, TCGETS, &ts);
	new_ts = ts;
	new_ts.c_lflag &= !ICANON;
	if (! echo)
		new_ts.c_lflag &= !ECHO;
	ioctl(0, TCSETS, &new_ts);

	size_t bytes;
	do
	{
		bytes = read(0, &c, 1);
	} while (/*bytes == 0 ||*/ c == '\r');

//	while ((c = getchar()) == '\r') {}
//	cin >> c;

	ioctl(0, TCSETS, &ts);

	if (bytes <= 0)
		return EOF;
	
	return c;
}

int getch(int iTimeoutInSeconds)
{
	return THE_getch(false,iTimeoutInSeconds);
}

int getche(int iTimeoutInSeconds)
{
	return THE_getch(true,iTimeoutInSeconds);
}
#endif
