#ifndef __OSD_SCREEN_HANLDER_H__
#define __OSD_SCREEN_HANLDER_H__

#include "../ScreenHandler.h"

class OSDScreenHandler: public ScreenHandler
{
protected:
	class WizardLogic *m_pWizardLogic;
	int m_nLightInDequeToAssign,m_nSensorInDequeToAssign; // To keep track of which light we're flashing, or sensor we're getting info on
	bool m_bLightsFlashThreadQuit,m_bLightsFlashThreadRunning;
	bool m_bHasVideoWizardFiles,m_bWizardIsRunning,m_bAlreadyPlaySeeAndHearMe,m_bAlreadyPlayFinalGreeting;
	time_t m_tWaitingForRegistration,m_tRegistered;

public:
	OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	~OSDScreenHandler();
	void DisableAllVideo();

	//
	//Video Wizard Screens
	//

	int m_dwMessageInterceptorCounter_ReportingChildDevices;

	virtual void SCREEN_VideoWizard(long PK_Screen);
	virtual bool VideoWizard_ObjectSelected(CallBackData *pData);
	virtual bool VideoWizard_OnTimer(CallBackData *pData);
	virtual bool VideoWizard_OnScreen(CallBackData *pData);

	//1. Users Wizard
	virtual void SCREEN_UsersWizard(long PK_Screen);
	virtual bool UsersWizard_ObjectSelected(CallBackData *pData);
	virtual bool UsersWizard_DatagridSelected(CallBackData *pData);
	virtual bool HandleAddUser(bool bErrorIfEmpty=true);

	//2. Location Wizard
	virtual void SCREEN_CountryWizard(long PK_Screen);
	virtual bool CountryWizard_ObjectSelected(CallBackData *pData);

	//3. Rooms Wizard
	virtual void SCREEN_RoomsWizard(long PK_Screen);
	virtual bool RoomsWizard_ObjectSelected(CallBackData *pData);
	virtual bool RoomsWizard_CapturedKeyboardBufferChanged(CallBackData *pData);
	virtual bool RoomsWizard_DatagridSelected(CallBackData *pData);

	// This room
	virtual void SCREEN_This_Room(long PK_Screen, bool bAlways);
	bool ThisRoom_ObjectSelected(CallBackData *pData);
	bool This_Room_GridSelected(CallBackData *pData);

	//4. TV Provider
	virtual void SCREEN_TV_provider(long PK_Screen);
	virtual bool TV_provider_ObjectSelected(CallBackData *pData);

	//5. TV Manuf.
	virtual void SCREEN_TV_Manufacturer(long PK_Screen);
	virtual bool TV_Manufacturer_ObjectSelected(CallBackData *pData);
	virtual bool TV_Manufacturer_GridSelected(CallBackData *pData);
	bool TV_OnTimer(CallBackData *pData);
	virtual bool TV_OnScreen(CallBackData *pData);

	//6. Receiver
	virtual void SCREEN_Receiver(long PK_Screen);
	virtual bool Receiver_ObjectSelected(CallBackData *pData);
	virtual bool Receiver_GridSelected(CallBackData *pData);
	virtual bool Receiver_OnTimer(CallBackData *pData);
	virtual bool Receiver_OnScreen(CallBackData *pData);

	//7. AV Devices
	virtual void SCREEN_AV_Devices(long PK_Screen);
	virtual bool AV_Devices_ObjectSelected(CallBackData *pData);
	virtual bool AV_Devices_CapturedKeyboardBufferChanged(CallBackData *pData);
	virtual bool AV_Devices_GridSelected(CallBackData *pData);

	//8. Done
	virtual void SCREEN_Wizard_Done(long PK_Screen);
	virtual void SCREEN_House_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line);
	virtual void SCREEN_Media_Player_Setup_Popup_Message(long PK_Screen, string sText, string sCommand_Line);

	// Lights
	virtual void SCREEN_LightsSetup(long PK_Screen);
	virtual bool LightsSetup_ObjectSelected(CallBackData *pData);
	virtual bool LightsSetup_SelectedGrid(CallBackData *pData);
	virtual bool LightsSetup_Intercepted(CallBackData *pData);
	virtual bool LightsSetup_OnScreen(CallBackData *pData);
	virtual bool LightsSetup_OnGotoScreen(CallBackData *pData);
	void LightsSetup_Timer();  // Our own thread so we can take a long time to wait for delivery confirmation
	virtual bool Lights_OnTimer(CallBackData *pData);  // A normal thread for checking if we have lights yet
	void HandleLightingScreen();

	//9. Alarm Panel
	virtual void SCREEN_AlarmPanel(long PK_Screen);
	virtual bool AlarmSetup_ObjectSelected(CallBackData *pData);
	virtual bool AlarmSetup_SelectedGrid(CallBackData *pData);
	virtual bool AlarmSetup_OnScreen(CallBackData *pData);
	virtual bool AlarmSetup_OnGotoScreen(CallBackData *pData);
	virtual bool Alarm_OnTimer(CallBackData *pData);  // A normal thread for checking if we have Alarm yet
	void HandleAlarmScreen();

	//10. VOIP Provider
	virtual void SCREEN_VOIP_Provider(long PK_Screen);
	bool VOIP_CapturedKeyboardBufferChanged(CallBackData *pData);
	bool VOIP_Provider_ObjectSelected(CallBackData *pData);

	virtual void SCREEN_mnuSpeedControl(long PK_Screen);	
	bool SpeedControlCreate(CallBackData *pData);
	bool SpeedControlDelete(CallBackData *pData);
	bool SpeedControlCustomRender(CallBackData *pData);

	// New AV Equipment
	// When working with new devices:
	//	VARIABLE_Misc_Data_1_CONST (12) = WizardLogic::m_dwPK_Manufacturer = Manufacturer ID
	//	VARIABLE_Misc_Data_2_CONST (13) = WizardLogic::m_dwPK_DeviceTemplate = Device Template

	bool NewAvDevice_ObjectSelected(int PK_DeviceCategory,ObjectInfoBackData *pObjectInfoData);
	virtual void SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack);
	virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage);

    //virtual void SCREEN_TVManufNotListed(long PK_Screen);
	//virtual bool SCREEN_TVManufNotListed_ObjectSelected(CallBackData *pData);

	//virtual bool SCREEN_TVConfirmInputsDiscrete_ObjectSelected(CallBackData *pData);

	//bool VolumeControlCustomRender(CallBackData *pData);
	// UI 2
	virtual void SCREEN_mnuVolume(long PK_Screen);
	bool VolumeControlCustomRender(CallBackData *pData);
	virtual void SCREEN_mnuLights(long PK_Screen);
	bool LightControlCustomRender(CallBackData *pData);
	virtual void SCREEN_mnuPlaybackControl(long PK_Screen);
	virtual bool PlayBackControlSelected(CallBackData *pData);
};

#endif
//__OSD_SCREEN_HANLDER_H__
