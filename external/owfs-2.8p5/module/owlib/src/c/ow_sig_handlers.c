/*
$Id: ow_sig_handlers.c,v 1.5 2010/01/25 16:08:30 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

#include <config.h>
#include "owfs_config.h"
#include "ow.h"

static void DefaultSignalHandler(int signo, siginfo_t * info, void *context)
{
	(void) context;
#if OW_MT
	if (info) {
		LEVEL_DEBUG
			("Signal handler for %d, errno %d, code %d, pid=%ld, self=%lu",
			 signo, info->si_errno, info->si_code, (long int) info->si_pid, pthread_self());
	} else {
		LEVEL_DEBUG("Signal handler for %d, self=%lu", signo, pthread_self());
	}
#else							/* OW_MT */
	if (info) {
		LEVEL_DEBUG("Signal handler for %d, errno %d, code %d, pid=%ld", signo, info->si_errno, info->si_code, (long int) info->si_pid);
	} else {
		LEVEL_DEBUG("Signal handler for %d", signo);
	}
#endif							/* OW_MT */
	return;
}

void set_signal_handlers(void (*handler)
	(int signo, siginfo_t * info, void *context))
{
	struct sigaction sa;
	int sigs[] = { SIGHUP, 0 };
	int i = 0;
	
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&(sa.sa_mask));

	while(sigs[i] > 0) {
		sa.sa_flags = SA_SIGINFO;
		if(handler != NULL) {
			sa.sa_sigaction = handler;
		} else {
			sa.sa_sigaction = DefaultSignalHandler;
		}
		if (sigaction(sigs[i], &sa, NULL) == -1) {
			LEVEL_DEFAULT("Cannot handle signal %d", sigs[i]);
			exit(1);
		}
		i++;
	}
}

void set_exit_signal_handlers(void (*exit_handler)
	(int signo, siginfo_t * info, void *context))
{
	struct sigaction sa;
	int sigs[] = { SIGINT, SIGTERM, 0 };
	int i = 0;
	
	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&(sa.sa_mask));

	while(sigs[i] > 0) {
		sa.sa_flags = SA_SIGINFO;
		sa.sa_sigaction = exit_handler;
		if (sigaction(sigs[i], &sa, NULL) == -1) {
			LEVEL_DEFAULT("Cannot handle signal %d", sigs[i]);
			exit(1);
		}
		i++;
	}
}
