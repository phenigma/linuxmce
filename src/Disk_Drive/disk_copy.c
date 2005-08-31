#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <stdio.h>

#define BLOCK_SIZE (10 * 1024 * 1024)
#define STR_SIZE 4095

//TODO: replace snprintf + write combination with printf override
// printf does a fstat() first and a write() later, depending if it's a terminal (instantly) or other kind of file (buffering)

char buffer[BLOCK_SIZE];
char str[STR_SIZE + 1];
int main(int argc, const char * argv[])
{
	int f, g;
	off64_t size, pos = 0, bytes, bytes2;
	float progress = 0, last_progress = 0;
	int error;

	if (argc != 3)
	{
		printf("Usage: disk_copy <src> <dest>\n");
		return 1;
	}
	
	f = open(argv[1], O_RDONLY);
	if (f == -1)
	{
		printf("Failed to open '%s': %s\n", argv[1], strerror(errno));
		return 1;
	}

	g = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
	if (g == -1)
	{
		printf("Failed to open '%s': %s\n", argv[1], strerror(errno));
		return 1;
	}

	size = lseek64(f, 0, SEEK_END);
	lseek64(f, 0, SEEK_SET);

	printf("Disc size: %Lu\n", size);
	fflush(stdout);
	
	while ((bytes = read(f, buffer, BLOCK_SIZE)) > 0)
	{
		bytes2 = write(g, buffer, bytes);
		if (bytes2 < bytes)
		{
			printf("Error writing to '%s': %s\n", argv[2], strerror(errno));
			return 1;
		}
		
		pos = lseek64(f, 0, SEEK_CUR);
		progress = pos * 1.0 / size * 100;
		if (last_progress != progress)
		{
			printf("Position: %Lu; Progress: %.2f%%\n", pos, progress);
			fflush(stdout);
			last_progress = progress;
		}
	}
	error = errno;

	close(g);
	close(f);

	if (bytes < 0)
	{
		printf("Error reading '%s': %s\n", argv[1], strerror(error));
		return 1;
	}
	
	printf("Done\n");
	return 0;
}
