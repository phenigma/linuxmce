#include "exec_grab_output.h"

#include <string>
#include <iostream>
using namespace std;

#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif

#ifdef WIN32
bool exec_output(const char * path, char * args[], string & sOutput)
{
	return true;
}
#else
bool exec_output(const char * path, char * args[], string & sOutput)
{
	int pid;
	int output[2];

	pipe(output);
	switch (pid = fork())
	{
		case 0: /* child */
			// treat stderr too someplace?
			close(output[0]);
			dup2(output[1], 1);
			execv(path, args);
			exit(254);
			break;
		case -1: /* error */
			return false;
			break;
		default: /* parent */
			close(output[1]);
			
			char buffer[4096];
			memset(buffer, 0, sizeof(buffer));
			while (read(output[0], buffer, sizeof(buffer) - 1) > 0)
			{
				sOutput += buffer;
				memset(buffer, 0, sizeof(buffer));
			}
			
			int status;
			waitpid(pid, &status, 0);
			status = WEXITSTATUS(status);

			if (status == 254)
				return false;
	}

	return true;
}
#endif

#ifdef ENABLE_MAIN
int main(int argc, char * argv[])
{
	char * args_default[] = {"(unused)", "/bin/cat", "/proc/cpuinfo", NULL};
	char ** args = args_default;

	if (argc > 1)
		args = argv;
	
	cout << "Executing " << args[1] << endl;
	string sOutput;
	if (exec_output(args[1], &args[1], sOutput))
	{
		cout << sOutput;
	}
	else
	{
		cout << "Execution failed" << endl;
	}
	
	return 0;
}
#endif
