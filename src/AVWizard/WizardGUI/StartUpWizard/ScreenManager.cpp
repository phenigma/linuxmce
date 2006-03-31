#include "ScreenManager.h"

#include "XRandrParser.h"
#include "XUtilities.h"

#include <iostream>

ScreenManager::ScreenManager()
	: Modes (NULL)
{
	ParseConfig();
}

ScreenManager::~ScreenManager()
{
	if(Modes)
		delete Modes;
}

void ScreenManager::ParseConfig()
{
	if (Modes)
		delete Modes;
	Modes = ParseXRandrOutput();
#ifdef DEBUG
	std::cout<<Modes->GetResolutionCount()<<std::endl;
	Modes->ToString();
#endif
}

bool ScreenManager::SetupHigherRefresh()
{
	if (!Modes)
		return false;

	Resolution* Current = Modes->GetResolution(Modes->GetDefaultResolutionIndex());
	int NewRefresh = Current->GetHigherRefresh(Modes->GetCurrentRefresh());

	if (NewRefresh>0)
	{
		SetXDisplayMode(Modes->GetCurrentWidth(), Modes->GetCurrentHeight(), NewRefresh);
		Modes->SetDefault(Modes->GetCurrentWidth(), Modes->GetCurrentHeight(), NewRefresh);
	}

	return true;
}

bool ScreenManager::SetupLowerRefresh()
{
	if (!Modes)
		return false;

	Resolution* Current = Modes->GetResolution(Modes->GetDefaultResolutionIndex());
	int NewRefresh = Current->GetLowerRefresh(Modes->GetCurrentRefresh());

	if (NewRefresh>0)
	{
		SetXDisplayMode(Modes->GetCurrentWidth(), Modes->GetCurrentHeight(), NewRefresh);
		Modes->SetDefault(Modes->GetCurrentWidth(), Modes->GetCurrentHeight(), NewRefresh);
	}

	return true;
}

bool ScreenManager::SetupHigherResolution()
{
	int ResID = Modes->GetDefaultResolutionIndex();
	int Count = Modes->GetResolutionCount();
	if (ResID>=Count-1)
		return false;

	Resolution *Higher = Modes->GetResolution(ResID+1);

	int CurentRefresh = Modes->GetCurrentRefresh();

	CurentRefresh = Higher->GetClosestRefresh(CurentRefresh);

	SetXDisplayMode(Higher->GetWidth(), Higher->GetHeight(), CurentRefresh);
	Modes->SetDefault(Higher->GetWidth(), Higher->GetHeight(), CurentRefresh);

	return true;
}

bool ScreenManager::SetupLowerResolution()
{
	int ResID = Modes->GetDefaultResolutionIndex();
	int Count = Modes->GetResolutionCount();
	if (ResID<=0)
		return false;

	Resolution *Lower = Modes->GetResolution(ResID-1);
	
	int CurentRefresh = Modes->GetCurrentRefresh();
	CurentRefresh = Lower->GetClosestRefresh(CurentRefresh);

	SetXDisplayMode(Lower->GetWidth(), Lower->GetHeight(), CurentRefresh);
	Modes->SetDefault(Lower->GetWidth(), Lower->GetHeight(), CurentRefresh);

	return true;
}
