/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef XRandrParser_H
#define XRandrParser_H

#include "ScreenModes.h"

#include <string>

/**
 * That classes read the output of command
 * $ xrandr -d :0 -q
 * and creates the ScreenModesObject coresponding to
 * that output that supports the X Server
 */
class XRandrParser
{
	static XRandrParser* Instance;

	/**
	 * Private constructor to make that class singleton (to not be
	 * instanced by other objects
	 */
	XRandrParser();

	/**
	 * Parse a line of the output and
	 * gives the coresponding Resolution object
	 * if UsedRefreshRate != -1 then the used refresh is on that line
	 * too that refresh rate says the current refresh rate
	 */
	Resolution* ParseLine(char* LineToParse, int& UsedRefreshRate);

	int StrToInt(char* String);

public:
	static XRandrParser* GetInstance();

	ScreenModes* ParseFile(char* FileName);
};

ScreenModes* ParseXRandrOutput();

#endif
