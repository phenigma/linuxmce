#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main()
{
	char c = 6;
	errno = 0;
	int r = ioctl(0, TIOCLINUX, &c);
	printf("c:%d r:%d e:%s\n", c, r, strerror(errno));

	return 0;
}
