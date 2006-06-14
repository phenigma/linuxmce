#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//#include "shift_keys.h"

int main()
{
	char c = 6;
	errno = 0;
	if (ioctl(0, TIOCLINUX, &c) == 0)
		printf("%d\n", c);

	return 0;
}
