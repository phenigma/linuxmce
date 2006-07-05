#include "WizardRemote.h"

#include "signal.h"

RemoteProcess::RemoteProcess()
{
	pid = 0;
	m_sCmd = "";
}

RemoteProcess::~RemoteProcess()
{
	if (pid > 0)
		kill(pid, SIGTERM);
}

void RemoteProcess::Start(string sCmd)
{
	if (pid >0)
		throw "Attempted to start another process";
	if (sCmd.size() == 0)
		return; // don't start anything; remote support will be disabled

	m_sCmd = sCmd;
	
	pid = fork();
	switch (pid)
	{
		case 0: /* child */
			chdir("/usr/pluto/bin");
			system(m_sCmd.c_str()); // too lazy to split and execv :)
			_exit(0);
			break;

		case -1: /* error */
			throw "Failed to fork remote control process";
			break;

		default: /* parent */
			break;
	}
}
