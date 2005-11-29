#include <stdlib.h>
#include <dvdread/dvd_reader.h>

int main()
{
	dvd_reader_t * dvd = NULL;

	dvd = DVDOpen("/dev/sr0");
    if (! dvd)
		return -1;
	DVDClose(dvd);
	return 0;
}
