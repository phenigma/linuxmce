#include "ProcessUtils.h"

#include <vector>
#include <map>

#include <errno.h>
#include <signal.h>

namespace ProcessUtils
{
	using std::map;
	using std::vector;

	typedef map<int, void*> MapPidToData;
	typedef map<string, MapPidToData> MapIdentifierToPidData;

	MapIdentifierToPidData mapIdentifierToPidData;
}

pthread_mutex_t mutexDataStructure = PTHREAD_MUTEX_INITIALIZER;

bool ProcessUtils::SpawnApplication(string sCmdExecutable, string sCmdParams, string sAppIdentifier, void *attachedData)
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

			if ( mapIdentifierToPidData.find(sAppIdentifier) == mapIdentifierToPidData.end() )
                mapIdentifierToPidData[sAppIdentifier] = map<int, void *>();

			mapIdentifierToPidData[sAppIdentifier][pid] = attachedData;
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

bool ProcessUtils::KillApplication(string sAppIdentifier, vector<void *> &associatedData)
{
	pthread_mutex_lock(&mutexDataStructure);
	MapIdentifierToPidData::iterator element = mapIdentifierToPidData.find(sAppIdentifier);

	if (element == mapIdentifierToPidData.end())
    {
        printf("ProcessUtils::KillApplication() No application '%s' found. None killed\n", sAppIdentifier.c_str());
		pthread_mutex_unlock(&mutexDataStructure);
		return false;
    }

	MapPidToData &mapPidsToData= element->second;
	printf("ProcessUtils::KillApplication(): Found %d '%s' applications. Killing them all (really)\n", mapPidsToData.size(), sAppIdentifier.c_str());

	associatedData.clear();
	MapPidToData::const_iterator itPidsToData = mapPidsToData.begin();
	while ( itPidsToData != mapPidsToData.end() )
	{
    	kill(itPidsToData->first, SIGKILL);
		associatedData.push_back(itPidsToData->second);
	}

	mapIdentifierToPidData.erase(element);
	pthread_mutex_unlock(&mutexDataStructure);
	return true;
}

bool ProcessUtils::ApplicationExited(int pid, string &associatedName, void *&associatedData, bool removeIt)
{
	pthread_mutex_lock(&mutexDataStructure);

	MapIdentifierToPidData::iterator applicationElement = mapIdentifierToPidData.begin();
	while ( applicationElement != mapIdentifierToPidData.end() )
	{
		printf("ProcessUtils::ApplicationExited() Looking at: %s\n", applicationElement->first.c_str());

		MapPidToData &mapPidsToData = applicationElement->second;
		MapPidToData::iterator itPidsToData = mapPidsToData.begin();
		while ( itPidsToData != mapPidsToData.end() && itPidsToData->first != pid )
		{
			printf("ProcessUtils::ApplicationExited() checking pid %d\n", itPidsToData->first);
			itPidsToData++;
		}

		if ( itPidsToData != mapPidsToData.end() )
		{
			associatedName = applicationElement->first;
			associatedData = itPidsToData->second;

			if ( removeIt )
				mapPidsToData.erase(itPidsToData);

			pthread_mutex_unlock(&mutexDataStructure);
			return true;
		}
		applicationElement++;
	}

	pthread_mutex_unlock(&mutexDataStructure);
	return false;
}
