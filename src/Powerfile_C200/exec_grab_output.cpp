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
	
	return 0;
}
