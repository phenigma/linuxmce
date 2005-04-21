#include "ProcessUtils.h"

#include <vector>
#include <map>

#include <errno.h>
#include <signal.h>

namespace ProcessUtils
{
	using std::map;
	using std::vector;

	typedef map<string, vector<int> > MapIdentifierToPids;

	MapIdentifierToPids mapIdentifierToPids;
}

pthread_mutex_t mutexDataStructure = PTHREAD_MUTEX_INITIALIZER;

bool ProcessUtils::SpawnApplication(string sCmdExecutable, string sCmdParams, string sAppIdentifier)
{
    if ( sAppIdentifier == "" )
        sAppIdentifier = "not named";

    if (sCmdExecutable == "" && sCmdParams == "")
    {
		printf("ProcessUtils::SpawnApplication() Received empty Executable and Parameters for '%s'\n", sAppIdentifier.c_str());
        return false;
    }

#ifndef WIN32
    //parse the args
    const int MaxArgs = 32;

    char * ptr;

	ptr = (char *) sCmdParams.c_str();
    char * args[MaxArgs];
    int i = 0;

	// this looks to complicated but i don;t have time to make it cleaner :-( mtoader@gmail.com)
    args[0] = (char *) sCmdExecutable.c_str();
    while ( *ptr && i < MaxArgs - 1)
    {
		if ( *ptr == ' ' || *ptr == '\t' )
			*ptr++ = 0;
		while ( *ptr && (*ptr == ' ' || *ptr == '\t') ) ptr++;  // skip the white spaces.
		if ( *ptr )
		{
			args[++i] = ptr; 			// put the next thing as a parameter
			while ( *ptr && *ptr != ' ' && *ptr != '\t' ) {
				if ( *ptr == '\\' ) ptr++; // skip quoted characters
				ptr++;  // skip to the next white space. (this doesn't take into account quoted parameters) )
			}
		}
    }

	args[++i] = 0;
	printf("ProcessUtils::SpawnApplication() Found %d arguments\n", i);

    for (int x = 0 ; x < i; x++)
	{
		char *pArgument 		= args[x];
		char *pUnquotedArgument = args[x];
		while ( *pArgument )
		{
			if ( *pArgument == '\\' ) pArgument++;
			*pUnquotedArgument++ = *pArgument++;
		}
		*pUnquotedArgument = 0;
		printf("ProcessUtils::SpawnApplication() Argument %d: %s\n", x, args[x]);
	}

    pid_t pid = fork();
    switch (pid)
    {
        case 0: //child
        {
            setenv("DISPLAY", ":0", 1);
            //now, exec the process
            printf("ProcessUtils::SpawnApplication(): Spawning\n");

            if ( execvp(args[0], args) == -1)
                exit(99);
        }

        case -1:
            printf("ProcessUtils::SpawnApplication() Error starting %s, err: %s\n", sCmdExecutable.c_str(), strerror(errno));
            return false;

		default:
			pthread_mutex_lock(&mutexDataStructure);
			printf("ProcessUtils::SpawnApplication() adding this %d pid to the spawned list for %s\n", pid, sAppIdentifier.c_str());
			mapIdentifierToPids[sAppIdentifier].push_back(pid);
			pthread_mutex_unlock(&mutexDataStructure);
            return true;
    }
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return true;
#endif
}

bool ProcessUtils::KillApplication(string sAppIdentifier)
{
	pthread_mutex_lock(&mutexDataStructure);
	MapIdentifierToPids::iterator element = mapIdentifierToPids.find(sAppIdentifier);

	if (element == mapIdentifierToPids.end())
    {
        printf("ProcessUtils::KillApplication() No application '%s' found. None killed\n", sAppIdentifier.c_str());
		pthread_mutex_unlock(&mutexDataStructure);
		return false;
    }

	vector<int> &our_vect = element->second;
	printf("ProcessUtils::KillApplication(): Found %d '%s' applications. Killing them all (really)\n", our_vect.size(), sAppIdentifier.c_str());

	vector<int>::const_iterator i;
    for (i = our_vect.begin(); i < our_vect.end(); i++)
    	kill(* i, SIGKILL);

	mapIdentifierToPids.erase(element);
	pthread_mutex_unlock(&mutexDataStructure);
	return true;
}

std::string ProcessUtils::FindApplicationFromPid(int pid, bool removeIt)
{
	pthread_mutex_lock(&mutexDataStructure);

	MapIdentifierToPids::iterator applicationElement = mapIdentifierToPids.begin();
	while ( applicationElement != mapIdentifierToPids.end() )
	{
		printf("ProcessUtils::FindApplicationFromPid() Looking at: %s\n", applicationElement->first.c_str());
		vector<int> &our_vect = applicationElement->second;
		vector<int>::iterator itPids = our_vect.begin();
		while ( itPids != our_vect.end() )
		{
			printf("ProcessUtils::FindApplicationFromPid() checking pid %d\n", *itPids);
			if ( *itPids != pid )
				itPids++;
			else
			{
				if ( removeIt )
					itPids = our_vect.erase(itPids);
				return applicationElement->first;
			}
		}
		applicationElement++;
	}

	pthread_mutex_unlock(&mutexDataStructure);
	return "";
}
