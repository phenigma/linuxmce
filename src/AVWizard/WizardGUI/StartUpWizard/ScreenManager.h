#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "ScreenModes.h"

class ScreenManager
{
	ScreenModes* Modes;
public:
	ScreenManager();
	~ScreenManager();

	void ParseConfig();

	bool SetupHigherRefresh();
	bool SetupLowerRefresh();

	bool SetupHigherResolution();
	bool SetupLowerResolution();

	ScreenModes* GetModes() {
		return Modes;
	}

};

#endif
