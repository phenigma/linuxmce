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

	// This is used to get elapsed time in milliseconds for quick comparissons.
	// clock is no good because it's only CPU time, not actual time.  timespec
	// is cumbersome.  You must call ResetMsTime to 'start' the stop watch, and 
	// then GetMsTime will report the elapsed time in milliseconds.  Because it's a
	// 32-bit integer, the values will roll over after 40 days, so ResetMsTime
	// must be called periodically.  The counter will start at the last 1 second break,
	// so it's not accurate for counting the milliseconds elapsed since Reset, only between
	// subsequent calls to GetMsTime
	extern unsigned long g_SecondsReset;
	void ResetMsTime();
	unsigned long GetMsTime();

};

#endif // PROCESSUTILS
