#include "PlutoUtils/ProcessUtils.h"

#include <string>
#include <iostream>
using namespace std;

#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif

int main(int argc, char * argv[])
{
	string sOutput;
	char * args[] = { "/usr/pluto/bin/Network_DisplaySettings.sh", "--orbiter", NULL };
	ProcessUtils::GetCommandOutput(args[0], &args[0], sOutput);
#if 0
	char * args_default[] = {"(unused)", "/bin/cat", "/proc/cpuinfo", NULL};
	char ** args = args_default;

	if (argc > 1)
		args = argv;
	
	cout << "Executing " << args[1] << endl;
	string sOutput;
	if (ProcessUtils::GetCommandOutput(args[1], &args[1], sOutput))
	{
		cout << sOutput;
	}
	else
	{
		cout << "Execution failed" << endl;
	}
#endif
	return 0;
}
