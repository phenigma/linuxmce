#ifndef __SCREEN_HANDLER_H__
#define __SCREEN_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "Gen_Devices/AllScreens.h"
#include "Orbiter.h"
#include "CallBackData.h"
//-----------------------------------------------------------------------------------------------------
typedef bool (ScreenHandler::*ScreenHandlerCallBack)(CallBackData *pData);
//-----------------------------------------------------------------------------------------------------
typedef class ScreenHandler * (* RAOP_FType) (class Orbiter *,  map<int,int> *p_MapDesignObj, Logger *);
//-----------------------------------------------------------------------------------------------------

class ScreenHandler : public ScreenHandlerBase
{
protected:
	Orbiter *m_pOrbiter;
	map<CallBackType, ScreenHandlerCallBack> m_mapCallBack;
	map<CallBackType, CallBackData *> m_mapCallBackData;

	virtual void DisplayMessageOnOrbiter(int PK_Screen,
		string sMessage, bool bPromptToResetRouter = false,
		string sTimeout = "0", bool bCantGoBack = false, 
		string sOption1 = "", string sMessage1 = "",
		string sOption2 = "", string sMessage2 = "", 
		string sOption3 = "", string sMessage3 = "",
		string sOption4 = "", string sMessage4 = "");

public: 
	ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	virtual ~ScreenHandler();

	//used to register as plugin
	virtual bool Register() { return 0; }

	//callback related stuff
	void RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack, 
		CallBackData *pCallBackData);
	void ResetCallBacks();
	ScreenHandlerCallBack m_mapCallBack_Find(CallBackType aCallBackType);
	CallBackData *m_mapCallBackData_Find(CallBackType aCallBackType);

	//helper functions
	int GetCurrentScreen_PK_DesignObj();
	void RefreshDatagrid(long PK_DesignObj_Datagrid);
	void BadGotoScreen(int PK_Screen);

	//screens functions
	virtual void GotoDesignObj(int PK_DesignObj);
	virtual void SCREEN_NewMacAddress(long PK_Screen, string sMacAddress, string sIpAddress);
	virtual void SCREEN_CDTrackCopy(long PK_Screen, string sPKUsers); 
	virtual void SCREEN_FileSave(long PK_Screen, string sDefaultUserValue, 
		string sPrivate, string sPublic, string sCaption);
	virtual void SCREEN_NewPhoneDetected(long PK_Screen, string sMacAddress, string sDescription);
	virtual void SCREEN_WhatModelMobileOrbiter(long PK_Screen, string sMacAddress);
	virtual void SCREEN_NewPlugAndPlayDevice(long PK_Screen, string sPK_Device, string sDescription, 
		string sComments);
	virtual void SCREEN_SensorsNotReady(long PK_Screen, string sDescription);
	virtual void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, 
		string sExitDelay, string sAlerts);
	//dialogs
	virtual void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors);
	virtual void SCREEN_DialogGenericNoButtons(long PK_Screen, string sDescription, string sPromptToResetRouter, 
		string sTimeout, string sCannotGoBack);
	virtual void SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_DeviceFrom, string sPK_RippingDevice);
	virtual void SCREEN_DialogCheckingDrive(long PK_Screen);
	virtual void SCREEN_DialogUnableToSavePlaylist(long PK_Screen);
	virtual void SCREEN_DialogPlaylistSaved(long PK_Screen);
	virtual void SCREEN_DialogUnableToLoadPlaylist(long PK_Screen);
	virtual void SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout	);
	virtual void SCREEN_DialogRippingInstructions(long PK_Screen);
	virtual void SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, 
		string sTimeout, string sCannotGoBack);
	virtual void SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors);
	virtual void SCREEN_DialogAskToResume(long PK_Screen, string sPK_DeviceFrom, string sPK_Device_MediaSource, 
		string sStreamID, string sPosition, string sUsers, string sPK_MediaType);
	virtual void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack);
	virtual void SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName);
	virtual void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine, 
		string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer);

	//Screens with locations
	virtual void SCREEN_Main(long PK_Screen, string sLocation);

	virtual void SCREEN_Lights(long PK_Screen, string sLocation);
	virtual void SCREEN_Media(long PK_Screen, string sLocation);
	virtual void SCREEN_Climate(long PK_Screen, string sLocation);
	virtual void SCREEN_Security(long PK_Screen, string sLocation);
	virtual void SCREEN_Telephony(long PK_Screen, string sLocation);
	virtual void SCREEN_Misc(long PK_Screen, string sLocation);

	virtual void SCREEN_popLights(long PK_Screen, string sLocation);
	virtual void SCREEN_popMedia(long PK_Screen, string sLocation);
	virtual void SCREEN_popClimate(long PK_Screen, string sLocation);
	virtual void SCREEN_popTelecom(long PK_Screen, string sLocation);
	virtual void SCREEN_popSecurity(long PK_Screen, string sLocation);

	virtual void SCREEN_NAS_Options(long PK_Screen, string sPK_DeviceTemplate, string sMacAddres, 
		string sIPAddress, string sPK_DHCPDevice);

	//cameras
	virtual void SCREEN_SingleCameraViewOnly(long PK_Screen, string sPKDevice);
	virtual void SCREEN_QuadViewCameras(long PK_Screen, string sPKDevicesList);

	virtual void SCREEN_New_Phone_Enter_Number(long PK_Screen, string sPhoneName, string sPK_Device);
};
//-----------------------------------------------------------------------------------------------------
#endif
