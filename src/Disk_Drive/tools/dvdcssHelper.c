#include <dvdread/dvd_reader.h>
#include <dvdread/ifo_read.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc<2) {
	printf("Usage: dvdcssHelper [-d] dvdpath\nwhere dvdpath is the path to DVD device or image\n-d - dump also all keys for disk\n");
	return 1;
    }

    char *szDVDPath = NULL;
    int bDumpKeys = 0;

    if ( strcmp("-d", argv[1])==0 ) {
	bDumpKeys = 1;
	if (argc<3) {
	    printf("Error: specify the dvdpath\n");
	    return 1;
	}
	else
	    szDVDPath = strdup(argv[2]);
    }
    else
	szDVDPath = strdup(argv[1]);

    // catching all stderr
//    dup2(stderr, stdin);

    setenv("DVDCSS_VERBOSE", "2", 1);
    setenv("DVDREAD_VERBOSE", "2", 1);

    dvd_reader_t *pDVD = DVDOpen(szDVDPath);

    if (!pDVD) {
	printf("Error: cannot open DVD\n");
	return 1;
    }

    // TODO read debug info here

    if (bDumpKeys) {
	dvd_file_t *pFile = DVDOpenFile(pDVD, 0, DVD_READ_MENU_VOBS);
	if (!pFile) {
	    printf("Error: cannot extract keys");
	    DVDClose(pDVD);
	    return 1;
	}

	DVDCloseFile(pFile);

	/*
	ifo_handle_t *ifo_file;
	tt_srpt_t *tt_srpt;

	ifo_file = ifoOpen( pDVD, 0 );
	if (ifo_file) {
	    printf("Error: cannot extract keys, stage 1");
	    DVDClose(pDVD);
	    return 1;
	}

	tt_srpt = ifo_file->tt_srpt;
	int i;

	for( i = 0; i < tt_srpt->nr_of_srpts; ++i ) {
	    int vtsnum = tt_srpt->title[ i ].title_set_nr;

	    ifo_handle_t *vts_file = ifoOpen( pDVD, vtsnum );

	    if (!vts_file) {
		printf("Error: cannot extract keys, stage 2-%i", i);
		DVDClose(pDVD);
		return 1;
	    }

	    ifoClose(vts_file);
	}

	ifoClose(ifo_file);
	*/
    }

    DVDClose(pDVD);
}
