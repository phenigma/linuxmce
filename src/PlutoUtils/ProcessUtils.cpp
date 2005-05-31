#include "ProcessUtils.h"

#include <vector>
#include <map>

#include <errno.h>
#include <signal.h>

#ifdef WIN32
	#include <pthread.h>
	#include <windows.h>
#endif
using namespace std;

#include "StringUtils.h"

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
    char * args[MaxArgs];

	string::size_type pos = 0;
    int i = 0;

	printf("ProcessUtils::SpawnApplication() sCMDExec %s parms: %s size: %d\n",
		sCmdExecutable.c_str(),sCmdParams.c_str(),(int) sCmdParams.size());

	// this looks to complicated but i don;t have time to make it cleaner :-( mtoader@gmail.com)
    args[0] = (char *) strdup(sCmdExecutable.c_str());
printf("dupped exec %s",args[0]);
	while ( pos!=string::npos && pos<sCmdParams.size() && i < MaxArgs - 1)
{
string s=StringUtils::Tokenize(sCmdParams,"\t",pos);
const char *ps=strdup(s.c_str());
printf("dupped arg %d %s\n",i,ps);

		args[++i] = strdup(s.c_str());
}

	args[++i] = 0;
	printf("ProcessUtils::SpawnApplication() Found %d arguments\n", i);

    for (int x = 0 ; x < i; x++)
		printf("ProcessUtils::SpawnApplication() Argument %d: %s\n", x, args[x]);

    pid_t pid = fork();
    switch (pid)
    {
        case 0: //child
        {
			// setenv("DISPLAY", ":0", 1);
            //now, exec the process
            printf("ProcessUtils::SpawnApplication(): Spawning\n");

            if ( execvp(args[0], args) == -1)
                exit(99);
        }

        case -1:
            printf("ProcessUtils::SpawnApplication() Error starting %s, err: %s\n", sCmdExecutable.c_str(), strerror(errno));
            return false;

		default:
			for(int i=0;i<i;++i)
				free(args[i]);

			printf("Freed args");

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

	vector<int> pidsArray;
	MapIdentifierToPidData::iterator element = mapIdentifierToPidData.find(sAppIdentifier);

	if (element == mapIdentifierToPidData.end())
    {
        printf("ProcessUtils::KillApplication() No application '%s' found. None killed\n", sAppIdentifier.c_str());
		pthread_mutex_unlock(&mutexDataStructure);
		return false;
    }

	MapPidToData &mapPidsToData = element->second;
	printf("ProcessUtils::KillApplication(): Found %d '%s' applications. Killing them all (really)\n", mapPidsToData.size(), sAppIdentifier.c_str());

	associatedData.clear();
	MapPidToData::const_iterator itPidsToData = mapPidsToData.begin();
	while ( itPidsToData != mapPidsToData.end() )
	{
		pidsArray.push_back(itPidsToData->first);
		associatedData.push_back(itPidsToData->second);
		itPidsToData++;
	}

	mapIdentifierToPidData.erase(element);
	pthread_mutex_unlock(&mutexDataStructure);

	printf("ProcessUtils::KillApplication(): Mutex unlocked Sending kill signal.\n");
	// we are doing the killing without the lock taken (since this killing might trigger another call into one of our functions which might cause a deadlock).
	vector<int>::const_iterator itPids = pidsArray.begin();
	while ( itPids != pidsArray.end() )
	{
#ifndef WIN32
		kill(*itPids, SIGKILL);
#endif
		itPids++;
	}

	return true;
}

bool ProcessUtils::ApplicationExited(int pid, string &associatedName, void *&associatedData, bool removeIt)
{
	// It can happen the thread that spawns hasn't yet had a chance to store the pid before it exits
	// Sleep half a sec just to be sure we give it some time.
	Sleep(500);

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

bool ProcessUtils::ApplicationIsLaunchedByMe(string applicationName)
{
	pthread_mutex_lock(&mutexDataStructure);

	if ( mapIdentifierToPidData.find(applicationName) == mapIdentifierToPidData.end() )
	{
		pthread_mutex_unlock(&mutexDataStructure);
		return false;
	}

	pthread_mutex_unlock(&mutexDataStructure);
	return true;
}
