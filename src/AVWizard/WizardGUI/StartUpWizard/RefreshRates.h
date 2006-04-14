//---------------------------------------------------------------------------

#ifndef RefreshRates_H
#define RefreshRates_H

#include <vector>

class RefreshRates
{
	std::vector<int> RefreshList;
public:
	RefreshRates();

	void AddRefreshRate(int RefreshRate);

	int GetRefreshRate(int RefreshRate);

	int GetCount();

	int GetRefreshByIndex(int Index);

	void ToString();
};

#endif
