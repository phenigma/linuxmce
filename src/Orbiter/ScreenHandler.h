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

	void DisplayMessageOnOrbiter(int PK_Screen,
		string sMessage, bool bPromptToResetRouter = false,
		string sTimeout = "0", bool bCantGoBack = false, 
		string sOption1 = "", string sMessage1 = "",
		string sOption2 = "", string sMessage2 = "", 
		string sOption3 = "", string sMessage3 = "",
		string sOption4 = "", string sMessage4 = "");

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
	//dialogs
	void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors);
	void SCREEN_DialogGenericNoButtons(long PK_Screen, string sDescription, string sPromptToResetRouter, 
		string sTimeout, string sCannotGoBack);
	void SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_DeviceFrom, string sPK_RippingDevice);
	void SCREEN_DialogCheckingDrive(long PK_Screen);
	void SCREEN_DialogUnableToSavePlaylist(long PK_Screen);
	void SCREEN_DialogPlaylistSaved(long PK_Screen);
	void SCREEN_DialogUnableToLoadPlaylist(long PK_Screen);
	void SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout	);
	void SCREEN_DialogRippingInstructions(long PK_Screen);
	void SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, 
		string sTimeout, string sCannotGoBack);
	void SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors);
	void SCREEN_DialogAskToResume(long PK_Screen, string sPK_DeviceFrom, string sPK_Device_MediaSource, 
		string sStreamID, string sPosition, string sUsers, string sPK_MediaType);
	void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack);
	void SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName);
	void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine, 
		string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer);
};
//-----------------------------------------------------------------------------------------------------
#endif