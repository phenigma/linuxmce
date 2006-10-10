/*
 * This file compiles only under Linux. Under Windows it gives an error if included, and it's not useful there anyway
 */
#ifndef __linux__
#error "This file is meant to be compiled under Linux"
#endif /*ifndef __linux__*/

#include "SignalHandler.h"

#include "App_Server.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <pthread.h>

using namespace DCE;

static App_Server *g_pAppServer = NULL;
static bool g_bQuit = false;
static pthread_t g_SignalHandler_Thread = 0;
/*
 * TODO: replace iSignalCounter with a IPC semaphore (which also increments),
 * to remove the busy loop below, because, with a semaphore, I can run a -1 operation
 * in the loop, which blocks if there's nothing to do, instead of constantly checking
 * a variable
 */
static int iSignalCounter = 0;

void sh(int i) /* signal handler */
{
	iSignalCounter++;
}

void SignalHandler_Action()
{
	if ( g_pAppServer && g_pAppServer->m_bQuit )
		return;
	PLUTO_SAFETY_LOCK(ap,g_pAppServer->m_AppMutex);

	iSignalCounter--;
	int status = 0;
	pid_t pid = 0;

	// wait for all children that exited, not just one of them
	// why: SIGCHLD is a POSIX standard signal, thus if a few children die very fast, we don't get one SIGCHLD per child
	//      we only get the first one, and if a child dies while we're in the handler, only one instance is queued
	//      not doing this, we risk leaving zombies behind in certain high-load situations
	// thanks go to the RLUG mailing list for the pointer
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		if (g_pAppServer)
		{
			int ExitStatus = WEXITSTATUS(status);
			int Signal = WIFSIGNALED(status) ? WTERMSIG(status) : 0;
			
			g_pPlutoLogger->Write(LV_STATUS, "SIGCHLD -- PID %d; Return code: %d; Signal: %d", pid, ExitStatus, Signal);
			// Send ourselves a message that calls the ApplicationExited function
			// We don't call it directly to avoid a deadlock if the SIGCHLD signal was received while in ProcessUtils::SpawnApplication
			DCE::CMD_Application_Exited CMD_Application_Exited(g_pAppServer->m_dwPK_Device, g_pAppServer->m_dwPK_Device, pid, ExitStatus);
			g_pAppServer->SendCommand(CMD_Application_Exited);
		}
	}
}

void * SignalHandler_Thread(void * Args)
{
	g_pAppServer = (App_Server *) Args;
	signal(SIGCHLD, sh); /* install signal handler */

	const struct timeval cTimeout = { tv_sec:0, tv_usec:500, };
	struct timeval timeout = cTimeout;

	while (! g_bQuit)
	{
		if (iSignalCounter > 0)
			SignalHandler_Action();
		select(0, NULL, NULL, NULL, &timeout);
		timeout = cTimeout;
	}

	signal(SIGCHLD, SIG_DFL);
	return NULL;
}

void SignalHandler_Start(App_Server *pApp_Server)
{
	pthread_create(&g_SignalHandler_Thread, NULL, SignalHandler_Thread, (void *) pApp_Server);
}

void SignalHandler_Stop()
{
	g_bQuit = true;

	if (g_SignalHandler_Thread != 0)
	{
		pthread_join(g_SignalHandler_Thread, NULL);
		g_SignalHandler_Thread = 0;
	}
}
