#include <stdio.h>
#include <dvdread/dvd_reader.h>

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <device>\n", argv[0]);
		return 1;
	}
	
	dvd_reader_t * dvd = NULL;

	dvd = DVDOpen(argv[1]);
    if (! dvd)
	{
		printf("Failed to open the DVD");
		return -1;
	}
	DVDClose(dvd);
	return 0;
}
