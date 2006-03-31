#ifndef ScreenModes_H
#define ScreenModes_H

#include <vector>

#include "Resolution.h"

class ScreenModes
{
	std::vector<Resolution> Resolutions;

	int CurrentWidth, CurrentHeight, CurrentRefresh;
public:
	ScreenModes();

	virtual ~ScreenModes();

	/**
     * Adds one new known resolution
	 */
	Resolution* AddNewResolution(int Width, int Height);

	/**
	 * Add a screen mode (resolution + refresh rates object)
	 * @result returns false if the object already exist
	 */
	bool AddScreenMode(Resolution Mode);

	/**
     * Gets the number of known resolution
	 */
	int GetResolutionCount();

	/**
     * Gets one specified resolution pointer
	 */
	Resolution* GetResolution(int Index); 

	/**
	 * Gets one specified resolution pointer
	 */
	int GetResolutionIndex(int Width, int Height); 

	/**
	 *	Clean up the resolutions stored
	 */
	void DeleteAllResolutions();

	/**
	 *	Setup the default screen mode
	 */
	void SetDefault(int Width, int Height, int Refresh);

	/**
	 *	That method gives a brief of the current object
	 */
	void ToString();

	/**
	 *	That method gives the default screen mode index
	 */
	int GetDefaultResolutionIndex();

	int GetCurrentWidth() {
		return this->CurrentWidth;
	}

	int GetCurrentHeight() {
		return this->CurrentHeight;
	}

	int GetCurrentRefresh() {
		return this->CurrentRefresh;
	}

};

bool operator < (Resolution Left, Resolution Right);

#endif
