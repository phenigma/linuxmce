/**
 *
 * @file ProcessUtils.h
 * @brief header file for the file utils namespace
 * @author
 * @todo notcommented
 *
 */

#ifndef PROCESSUTILS
#define PROCESSUTILS

#include <string>
#include <vector>

namespace ProcessUtils
{
	using std::string;
	using std::vector;

	// sCmdParams are tab delimited
	int SpawnApplication(string sCmdExecutable, string sCmdParams, string strAppIdentifier, void *attachedData = NULL, bool bLogOutput = true);

	bool KillApplication(string sAppIdentifier, vector<void *> &associatedData);

	bool ApplicationExited(int pid, string &associatedName, void *&associatedData, bool removeIt = true);

	bool ApplicationIsLaunchedByMe(string applicationName);

	bool SendKeysToProcess(string sAppIdentifier,string sKeys);

	void KillAllApplications();  // Kill any applications we have spawned

	/** Execute a command with execv and return its stdout */
	bool GetCommandOutput(const char * path, char * args[], string & sOutput);
};

#endif // PROCESSUTILS
