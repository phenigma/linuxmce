#include "WizardRemote.h"

#include "signal.h"
#include <iostream>
using namespace std;

RemoteProcess::RemoteProcess()
{
	pid = 0;
	m_sCmd = "";
}

RemoteProcess::~RemoteProcess()
{
#ifndef WIN32
	if (pid > 0)
		kill(pid, SIGTERM);
#endif
}

void RemoteProcess::Start(string sCmd)
{
	cout << "RemoteProcess::Start: sCmd='" << sCmd << "'" << endl;
	if (pid > 0)
		throw "Attempted to start another process";
	if (sCmd.size() == 0)
		return; // don't start anything; remote support will be disabled

	m_sCmd = sCmd;

#ifndef WIN32
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
#endif
}
