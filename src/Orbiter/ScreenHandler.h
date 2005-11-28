#ifndef __SCREEN_HANDLER_H__
#define __SCREEN_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "Gen_Devices/AllScreens.h"
#include "Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class ScreenHandler : public ScreenHandlerBase
{
protected:
	Orbiter *m_pOrbiter;

public: 
	ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);

	void GotoDesignObj(int PK_DesignObj);
	void SCREEN_NewMacAddress(long PK_Screen, string sMacAddress, string sIpAddress);
	void SCREEN_CDTrackCopy(long PK_Screen, string sPKUsers); 
	void SCREEN_FileSave(long PK_Screen, string sPrivate, string sPublic, string sCaption);
	void SCREEN_NewPhoneDetected(long PK_Screen, string sMacAddress, string sDescription);
	void SCREEN_WhatModelMobileOrbiter(long PK_Screen, string sMacAddress);
	void SCREEN_NewPlugAndPlayDevice(long PK_Screen, string sPK_Device, string sDescription, 
		string sComments);
	void SCREEN_SensorsNotReady(long PK_Screen, string sDescription);
	void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, 
		string sExitDelay, string sAlerts);
};
//-----------------------------------------------------------------------------------------------------
#endif