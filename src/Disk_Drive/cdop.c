#include <linux/cdrom.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

inline int usage()
{
	printf("Usage: cdop <device> {lock|unlock}\n");
	return 1;
}

int main(int argc, const char * argv[])
{
	int dev;
	int state;

	if (argc != 3)
	{
		return usage();
	}

	if (strcmp(argv[2], "lock") == 0)
		state = 1;
	else if (strcmp(argv[2], "unlock") == 0)
		state = 0;
	else
		return usage();
	
	dev = open(argv[1], O_RDONLY);
	if (dev == -1)
	{
		perror("Failed to open cdrom");
		return 2;
	}
	
	if (ioctl(dev, CDROM_LOCKDOOR, state) == -1)
	{
		perror("Failed to perform operation");
		return 3;
	}
	
	close(dev);

	return 0;
}
