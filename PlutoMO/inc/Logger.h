#ifndef __LOGGER_H__
#define __LOGGER_H__
//----------------------------------------------------------------------------------------------
#include <utf.h>
#include <coeccntx.h>
#include <coemain.h>

#include "VIPShared/VIPIncludes.h"
//----------------------------------------------------------------------------------------------
#define ENABLE_LOG
//----------------------------------------------------------------------------------------------
// CONSTANTS
// UID of view
const TUid KCPlutoLoggerId = {102};
//----------------------------------------------------------------------------------------------
#define LOG(s) { ((SymbianLogger *)CCoeEnv::Static(KCPlutoLoggerId))->Write(s); }
#define LOGN(s) { ((SymbianLogger *)CCoeEnv::Static(KCPlutoLoggerId))->Write(s, false); }

class SymbianLogger : public CCoeStatic
{
	RFile file;
	RFs   aFs;

public: 

	SymbianLogger(string FileName, TUid aUid, TScope scop = EThread);
	~SymbianLogger();

	void Write(int Value,  bool bAddTime = true);
	void Write(char *log,  bool bAddTime = true);
	void Write(string log, bool bAddTime = true);
};
//----------------------------------------------------------------------------------------------
#endif __LOGGER_H__