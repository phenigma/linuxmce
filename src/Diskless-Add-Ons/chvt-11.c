/* extremely light program for switching to tty11 */
/* inspired by chvt.c */
#include <fcntl.h>

#define VT_ACTIVATE 0x5606
#define VT_WAITACTIVE 0x5607

int main()
{
	int fd;

	fd = open("./dev/console", O_RDWR);
	ioctl(fd, VT_ACTIVATE, 11);
	ioctl(fd, VT_WAITACTIVE, 11);
	return 0;
}
