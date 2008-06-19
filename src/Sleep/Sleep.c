#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <milliseconds>\n", argv[0]);
		return 1;
	}
	int milliseconds = atoi(argv[1]) * 1000;
	usleep(milliseconds);

	return 0;
}
