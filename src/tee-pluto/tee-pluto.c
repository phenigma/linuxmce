#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void usage()
{
	fprintf(stderr, "Usage: %s [-a] file\n", "tee-pluto");
}

int main(int argc, char * argv[])
{
	int i;
	int append = 0;
	int optc;

	while ((optc = getopt(argc, argv, "a")) != -1)
	{
		switch (optc)
		{
			case 'a':
				append = 1;
				break;

			default:
				usage();
				return 1;
		}
    }

	if (argc - optind < 1)
	{
		usage();
		return 1;
	}

	char * filename = argv[optind];
	FILE * f;

	if (append)
		f = fopen(filename, "ab");
	else
		f = fopen(filename, "wb");

	if (!f)
	{
		fprintf(stderr, "ERROR: Failed to open output file: %s\n", strerror(errno));
		return 1;
	}

	ssize_t bytes_read;
	char buffer[BUFSIZ];
	while (1)
	{
		bytes_read = read(0, buffer, sizeof(buffer));
#ifdef EINTR
		if (bytes_read < 0 && errno == EINTR)
			continue;
#endif
		if (bytes_read <= 0)
			break;

		if (fwrite(buffer, 1, bytes_read, f) != bytes_read)
		{
			fprintf(stderr, "ERROR: Failed to write to output file: %s\n", strerror(errno));
			return 1;
		}
		if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read)
		{
			fprintf(stderr, "ERROR: Failed to write to stdout: %s\n", strerror(errno));
			return 1;
		}
	}

	fclose(f);
	return 0;
}
