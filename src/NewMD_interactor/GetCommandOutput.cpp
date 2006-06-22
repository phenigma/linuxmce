#include <unistd.h>
#include <sys/wait.h>

#include "GetCommandOutput.h"

bool GetCommandOutput(const char * path, char * args[], string & sOutput)
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
			_exit(254);
			/* Rationale for _exit instead of exit:
			 * exit calls atexit-registered functions and X registers some stuff
			 * this causes the Orbiter to lose the connection to X
			 * but this child doesn't register any atexit functions, nor does it make any output if exec fails
			 */
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

			close(output[0]);
			if (status == 254)
				return false;
	}

	return true;
}
