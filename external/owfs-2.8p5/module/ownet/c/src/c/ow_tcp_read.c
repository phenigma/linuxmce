/*
$Id: ow_tcp_read.c,v 1.3 2010/05/10 02:11:50 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

/* ow_net holds the network utility routines. Many stolen unashamedly from Steven's Book */
/* Much modification by Christian Magnusson especially for Valgrind and embedded */
/* non-threaded fixes by Jerry Scharf */

#include <config.h>
#include "owfs_config.h"
#include "ow.h"

/* Read "n" bytes from a descriptor. */
/* Stolen from Unix Network Programming by Stevens, Fenner, Rudoff p89 */
ssize_t tcp_read(int file_descriptor, void *vptr, size_t n, const struct timeval * ptv)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	//printf("NetRead attempt %d bytes Time:(%ld,%ld)\n",(int)n,ptv->tv_sec,ptv->tv_usec ) ;
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		int rc;
		fd_set readset;
		struct timeval tv = { ptv->tv_sec, ptv->tv_usec, };

		/* Initialize readset */
		FD_ZERO(&readset);
		FD_SET(file_descriptor, &readset);

		/* Read if it doesn't timeout first */
		rc = select(file_descriptor + 1, &readset, NULL, NULL, &tv);
		if (rc > 0) {
			/* Is there something to read? */
			if (FD_ISSET(file_descriptor, &readset) == 0) {
				return -EIO;	/* error */
			}
			//update_max_delay(pn);
			if ((nread = read(file_descriptor, ptr, nleft)) < 0) {
				if (errno == EINTR) {
					errno = 0;	// clear errno. We never use it anyway.
					nread = 0;	/* and call read() again */
				} else {
					ERROR_DATA("Network data read error\n");
					return (-1);
				}
			} else if (nread == 0) {
				break;			/* EOF */
			}
			//Debug_Bytes( "NETREAD",ptr, nread ) ;
			nleft -= nread;
			ptr += nread;
		} else if (rc < 0) {	/* select error */
			if (errno == EINTR) {
				/* select() was interrupted, try again */
				continue;
			}
			ERROR_DATA("Selection error (network)\n");
			return -EINTR;
		} else {				/* timed out */
			LEVEL_CONNECT("TIMEOUT after %d bytes\n", n - nleft);
			return -EAGAIN;
		}
	}
	return (n - nleft);			/* return >= 0 */
}

void tcp_read_flush(int file_descriptor)
{
	ASCII buffer[16];
	ssize_t nread;
	int flags = fcntl(file_descriptor, F_GETFL, 0);

	// Apparently you can test for GET_FL success like this
	// see http://www.informit.com/articles/article.asp?p=99706&seqNum=13&rl=1
	if (flags < 0)
		return;

	if (fcntl(file_descriptor, F_SETFL, flags | O_NONBLOCK) < 0)
		return;

	while ((nread = read(file_descriptor, (BYTE *) buffer, 16)) > 0) {
		Debug_Bytes("tcp_read_flush", (BYTE *) buffer, nread);
		continue;
	}

	fcntl(file_descriptor, F_SETFL, flags);
}
