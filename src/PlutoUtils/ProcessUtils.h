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

namespace ProcessUtils
{
	using std::string;

	bool SpawnApplication(string sCmdExecutable, string sCmdParams, string strAppIdentifier);
	bool KillApplication(string sAppIdentifier);
};

#endif // PROCESSUTILS
