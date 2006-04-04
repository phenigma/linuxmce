#ifndef NEWMD_INTERACTOR_COMMON_H
#define NEWMD_INTERACTOR_COMMON_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/reboot.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#define INTERACTOR_PORT 7238

// set close-on-exec flag
void set_close_on_exec(int sock)
{
	long sock_flags;

	sock_flags = fcntl(sock, F_GETFD);
	sock_flags |= FD_CLOEXEC;
	fcntl(sock, F_SETFD, sock_flags);
}
#endif
