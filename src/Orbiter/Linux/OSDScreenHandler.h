#ifndef __OSD_SCREEN_HANLDER_H__
#define __OSD_SCREEN_HANLDER_H__

#include "../ScreenHandler.h"

class OSDScreenHandler: public ScreenHandler
{
protected:
	class WizardLogic *m_pWizardLogic;
	int m_nLightInDequeToAssign; // To keep track of which light we're flashing
	bool m_bLightsFlashThreadQuit,m_bLightsFlashThreadRunning;

public:
	OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	~OSDScreenHandler();

	//
	//Video Wizard Screens
	//

	int m_dwMessageInterceptorCounter_ReportingChildDevices;

	//1. Users Wizard
	virtual void SCREEN_UsersWizard(long PK_Screen);
	virtual bool UsersWizard_ObjectSelected(CallBackData *pData);
	virtual bool UsersWizard_DatagridSelected(CallBackData *pData);
	virtual bool HandleAddUser();

	//2. Location Wizard
	virtual void SCREEN_CountryWizard(long PK_Screen);
	virtual bool CountryWizard_ObjectSelected(CallBackData *pData);

	//3. Rooms Wizard
	virtual void SCREEN_RoomsWizard(long PK_Screen);
	virtual bool RoomsWizard_ObjectSelected(CallBackData *pData);
	virtual bool RoomsWizard_CapturedKeyboardBufferChanged(CallBackData *pData);
	virtual bool RoomsWizard_DatagridSelected(CallBackData *pData);

	//4. TV Provider
	virtual void SCREEN_TV_provider(long PK_Screen);
	virtual bool TV_provider_ObjectSelected(CallBackData *pData);

	//5. A/V Connection
	virtual void SCREEN_TV_Manufacturer(long PK_Screen);
	virtual bool TV_Manufacturer_ObjectSelected(CallBackData *pData);
	virtual bool TV_Manufacturer_GridSelected(CallBackData *pData);

	//6. Receiver
	virtual void SCREEN_Receiver(long PK_Screen);
	virtual bool Receiver_ObjectSelected(CallBackData *pData);
	virtual bool Receiver_GridSelected(CallBackData *pData);

	//7. AV Devices
	virtual void SCREEN_AV_Devices(long PK_Screen);
	virtual bool AV_Devices_ObjectSelected(CallBackData *pData);

	//8. Lights Setup
	virtual void SCREEN_LightsSetup(long PK_Screen);
	virtual bool LightsSetup_ObjectSelected(CallBackData *pData);
	virtual bool LightsSetup_SelectedGrid(CallBackData *pData);
	virtual bool LightsSetup_Intercepted(CallBackData *pData);
	virtual bool LightsSetup_OnScreen(CallBackData *pData);
	virtual bool LightsSetup_OnGotoScreen(CallBackData *pData);
	void LightsSetup_Timer();

	//9. Alarm Panel
	virtual void SCREEN_AlarmPanel(long PK_Screen);
	bool AlarmPanel_ObjectSelected(CallBackData *pData);
	bool AlarmPanel_OnTimer(CallBackData *pData);
	bool AlarmPanel_DatagridSelected(CallBackData *pData);
	bool AlarmPanel_CapturedKeyboardBufferChanged(CallBackData *pData);
	bool AlarmPanel_Intercepted(CallBackData *pData);

	//10. VOIP Provider
	virtual void SCREEN_VOIP_Provider(long PK_Screen);
	bool VOIP_Provider_ObjectSelected(CallBackData *pData);

    // RoomsWizard callbacks
    bool RoomsWizardCreate( CallBackData *pData );
    bool RoomsWizardDelete( CallBackData *pData );
    bool RoomsWizardRefresh( CallBackData *pData );
	
	virtual void SCREEN_tempmnuspeed(long PK_Screen);	
	bool SpeedControlCreate(CallBackData *pData);
	bool SpeedControlDelete(CallBackData *pData);
	bool m_bSpeedControlCreated;
	//NOTE: for refresh, see SpeedMouseHandler::Update
};

#endif
//__OSD_SCREEN_HANLDER_H__
