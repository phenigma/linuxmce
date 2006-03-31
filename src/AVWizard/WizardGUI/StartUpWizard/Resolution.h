//---------------------------------------------------------------------------

#ifndef Resolution_H
#define Resolution_H

#include "RefreshRates.h"

class Resolution
{
	int Width, Height;

    RefreshRates Refreshes;
public:
	Resolution(int Width, int Height);

	void AddRefreshRate(int RefreshRate);

	int GetWidth() {
		return Width;
	}

	int GetHeight() {
		return Height;
	}

	/**
	 *	Gives the closest refresh rate relative to the value stored
	 *	That function is used that when is changed the screen resolution will offer the 
	 *	same refresh (as much as possible)
	 */
	int GetClosestRefresh(int Refresh);

	/**
	 *	Gives the closest higher refresh from a given refresh
	 *	If that refresh doesn't exist, that function will return -1
	 */
	int GetHigherRefresh(int Refresh);

	/**
	 *	Gives the closest lower refresh from a given refresh
	 *	If that refresh doesn't exist, that function will return -1
	 */
	int GetLowerRefresh(int Refresh);


	void ToString();
};

#endif
