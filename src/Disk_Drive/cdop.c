#include <linux/cdrom.h>
#include <fcntl.h>
#include <string.h>

inline int usage()
{
	printf("Usage: cdop <lock|unlock>\n");
	return 1;
}

int main(int argc, const char * argv[])
{
	int dev;
	int state;

	if (argc != 2)
	{
		return usage();
	}

	if (strcmp(argv[1], "lock") == 0)
		state = 1;
	else if (strcmp(argv[1], "unlock") == 0)
		state = 0;
	else
		return usage();
	
	dev = open("/dev/cdrom", O_RDONLY);
	ioctl(dev, CDROM_LOCKDOOR, state);
	close(dev);
}
