#include "ScreenHandler.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include "WizardLogic.h"
//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : ScreenHandlerBase(p_MapDesignObj)
{
	m_pOrbiter = pOrbiter;
	m_pWizardLogic = new WizardLogic(pOrbiter);

	if(!m_pWizardLogic->Setup())
	{
		//do something here; unable to connect to db
	}

	m_nPK_Device_TVProvider_External = 0;
}
//-----------------------------------------------------------------------------------------------------
ScreenHandler::~ScreenHandler() 
{
	m_pOrbiter = NULL;
	//delete m_pWizardLogic;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack, 
	CallBackData *pCallBackData)
{
	m_mapCallBack[aCallBackType] = aScreenHandlerCallBack;
	m_mapCallBackData[aCallBackType] = pCallBackData;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::ResetCallBacks()
{
	m_mapCallBack.clear();

	for(map<CallBackType, CallBackData *>::iterator it = m_mapCallBackData.begin(); it != m_mapCallBackData.end(); it++)
	{
		CallBackData *pCallBackData = it->second;
		delete pCallBackData;
	}
	m_mapCallBackData.clear();
}
//-----------------------------------------------------------------------------------------------------
ScreenHandlerCallBack ScreenHandler::m_mapCallBack_Find(CallBackType aCallBackType)	
{ 
	map<CallBackType, ScreenHandlerCallBack>::iterator it = m_mapCallBack.find(aCallBackType); 
	return it == m_mapCallBack.end() ? NULL : it->second; 
}
//-----------------------------------------------------------------------------------------------------
CallBackData *ScreenHandler::m_mapCallBackData_Find(CallBackType aCallBackType)	
{ 
	map<CallBackType, CallBackData *>::iterator it = m_mapCallBackData.find(aCallBackType); 
	return it == m_mapCallBackData.end() ? NULL : it->second; 
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GotoDesignObj(int PK_DesignObj)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(PK_DesignObj), "", "", false, false);
}
//-----------------------------------------------------------------------------------------------------
int ScreenHandler::GetCurrentScreen_PK_DesignObj()
{
	return m_pOrbiter->m_pScreenHistory_Current->m_pObj->m_iBaseObjectID;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewMacAddress(long PK_Screen, string sMacAddress, string sIpAddress) 
{ 
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sIpAddress);
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sMacAddress, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CDTrackCopy(long PK_Screen, string sPKUsers) 
{ 
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPKUsers);
	ScreenHandlerBase::SCREEN_CDTrackCopy(PK_Screen, sPKUsers);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileSave(long PK_Screen, string sDefaultUserValue, string sPrivate, 
									string sPublic, string sCaption)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPrivate);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sPublic);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sDefaultUserValue);
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sCaption, TEXT_STATUS_CONST);
	ScreenHandlerBase::SCREEN_FileSave(PK_Screen, sDefaultUserValue, sPrivate, sPublic, sCaption);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPhoneDetected(long PK_Screen, string sMacAddress, string sDescription)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMacAddress);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sDescription);
	ScreenHandlerBase::SCREEN_NewPhoneDetected(PK_Screen, sMacAddress, sDescription);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_WhatModelMobileOrbiter(long PK_Screen, string sMacAddress)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMacAddress);
	ScreenHandlerBase::SCREEN_WhatModelMobileOrbiter(PK_Screen, sMacAddress);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPlugAndPlayDevice(long PK_Screen, string sPK_Device, 
												   string sDescription, string sComments)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sComments);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPK_Device);
	ScreenHandlerBase::SCREEN_NewPlugAndPlayDevice(PK_Screen, sPK_Device, sDescription, sComments);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_SensorsNotReady(long PK_Screen, string sDescription)
{
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sDescription, 
		TEXT_TRIPPED_SENSOR_CONST);	
	ScreenHandlerBase::SCREEN_SensorsNotReady(PK_Screen, sDescription);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, 
						   string sExitDelay, string sAlerts)
{
	string sPK_DesignObj = StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen));

	m_pOrbiter->CMD_Set_Graphic_To_Display(sPK_DesignObj + ".0.0." + 
		StringUtils::itos(DESIGNOBJ_icoHouseStatusIndicator_CONST), sPK_HouseMode);
	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sHouseModeTime, TEXT_House_Mode_Time_CONST);

	if(sExitDelay != "")	
		m_pOrbiter->CMD_Select_Object(sPK_DesignObj, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), sExitDelay);

	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sAlerts, TEXT_Alerts_Placeholder_CONST);
	ScreenHandlerBase::SCREEN_ModeChanged(PK_Screen, sPK_HouseMode, sHouseModeTime, sExitDelay, sAlerts);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>" + sErrors);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogGenericNoButtons(long PK_Screen, string sDescription, string sPromptToResetRouter, 
								   string sTimeout, string sCannotGoBack)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, sPromptToResetRouter == "1", sTimeout, sCannotGoBack == "1");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_DeviceFrom, string sPK_RippingDevice)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_ripping_cant_play_disc_CONST) + "%>",
		false, "10", true, 
		//"Abort ripping text"
		"<%=T" + StringUtils::itos(TEXT_Abort_ripping_CONST) + "%>",
		//"Abort ripping message"
		sPK_DeviceFrom + " " + sPK_RippingDevice + " 1 " + StringUtils::itos(COMMAND_Abort_Ripping_CONST),
		//"Monitor progress text"
		"<%=T" + StringUtils::itos(TEXT_Monitor_progress_CONST) + "%>",
		//"Monitor progress message"
		"0 " + StringUtils::itos(DEVICETEMPLATE_This_Orbiter_CONST) + 
		" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + 
		StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + StringUtils::itos(DESIGNOBJ_mnuPendingTasks_CONST)
	);
	return;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCheckingDrive(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Checking_drive_CONST) + "%>", false, "20");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogUnableToSavePlaylist(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "Unable to save playlist", false, "10", true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogPlaylistSaved(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Playlist_Saved_CONST) + "%>");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogUnableToLoadPlaylist(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, "Unable to load playlist", false, "10", true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, sTimeout, false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogRippingInstructions(long PK_Screen)
{
	DisplayMessageOnOrbiter(PK_Screen, 
		"<%=T" + StringUtils::itos(TEXT_Ripping_Instructions_CONST) + "%>",
        false, "40", false, 
		//Monitor progress text
		"<%=T" + StringUtils::itos(TEXT_Monitor_progress_CONST) + "%>",
		//the command
		"0 " + StringUtils::itos(DEVICETEMPLATE_This_Orbiter_CONST) + 
		" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + 
		StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + 
		StringUtils::itos(DESIGNOBJ_mnuPendingTasks_CONST) 
	);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, 
							   string sTimeout, string sCannotGoBack)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, sPromptToResetRouter == "1", sTimeout, sCannotGoBack == "1");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Cannot_bookmark_CONST) + 
		"%> " + sErrors);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogAskToResume(long PK_Screen, string sPK_DeviceFrom, 
	string sPK_Device_MediaSource, string sStreamID, string sPosition, string sUsers, string sPK_MediaType)
{
	string sMessageToResume = sPK_DeviceFrom + " " + sPK_Device_MediaSource
		+ " 1 " + StringUtils::itos(COMMAND_Set_Media_Position_CONST) + " " 
		+ StringUtils::itos(COMMANDPARAMETER_StreamID_CONST) + " " + sStreamID + " "
		+ StringUtils::itos(COMMANDPARAMETER_MediaPosition_CONST) + " \"" + sPosition + "\"";

	string sMessageToGoToRemote = sPK_DeviceFrom + " " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device)
		+ " 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " 
		+ StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " <%=NP_R%>";

	string sMessageToSetPreference = StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + sPK_DeviceFrom
		+ " 1 " + StringUtils::itos(COMMAND_Set_Auto_Resume_Options_CONST) + " " 
		+ StringUtils::itos(COMMANDPARAMETER_PK_Users_CONST) + " " + sUsers + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_MediaType_CONST) + " " + sPK_MediaType + " "
		+ StringUtils::itos(COMMANDPARAMETER_Value_To_Assign_CONST);

	DisplayMessageOnOrbiter(PK_Screen, 
		"<%=T" + StringUtils::itos(TEXT_Ask_to_resume_CONST) + "%>",
		false, "30", true,
		"<%=T" + StringUtils::itos(TEXT_YES_CONST) + "%>",sMessageToResume + "\n" + sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_NO_CONST) + "%>",sMessageToGoToRemote,
		"<%=T" + StringUtils::itos(TEXT_Always_Resume_CONST) + "%>",sMessageToResume + "\n" + 
			sMessageToGoToRemote + "\n" + sMessageToSetPreference + " A",
		"<%=T" + StringUtils::itos(TEXT_Never_Resume_CONST) + "%>",sMessageToGoToRemote + "\n" + 
			sMessageToSetPreference + " N");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack)
{
	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, "0", false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::DisplayMessageOnOrbiter(int PK_Screen,
	string sMessage, bool bPromptToResetRouter,string sTimeout, bool bCantGoBack, 
	string sOption1, string sMessage1, string sOption2, string sMessage2, 
	string sOption3, string sMessage3, string sOption4, string sMessage4)
{
	string sPK_DesignObj = StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen));

	//if(sMessage == "")
	//	sMessage = "Unable to save playlist";

	m_pOrbiter->CMD_Set_Timeout(sPK_DesignObj, sTimeout);
//	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, "", "", false, false);

	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, "", "", false, bCantGoBack );
	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sMessage, TEXT_STATUS_CONST);

	if(bPromptToResetRouter)
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );

	if(sOption1.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), sOption1, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMessage1);
	}
	if(sOption2.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), sOption2, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sMessage2);
	}
	if(sOption3.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), sOption3, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sMessage3);
	}
	if(sOption4.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), sOption4, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, sMessage4);
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPhoneName);
	DisplayMessageOnOrbiter(PK_Screen, sInstructions, false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine, 
	string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer)
{
	string sYesMessage = 
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + sPK_DeviceFrom  + " " + 
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Send_File_To_Phone_CONST) + " " + 
		StringUtils::itos(COMMANDPARAMETER_Mac_address_CONST) + " '" + sMacAddress + "'" + " " + 
		StringUtils::itos(COMMANDPARAMETER_Command_Line_CONST) + " '" + sCommandLine + "'" + " " + 
		StringUtils::itos(COMMANDPARAMETER_App_Server_Device_ID_CONST) + " " + sPK_Device_AppServer;

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPhoneName);
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_FAILED_TO_UPLOAD_SIS_FILE_CONST) + "%>",
		false, "0", false, 
		//Yes button
		"Yes",      
		//Yes command
		sYesMessage,
		//No button
		"No",
		//No command
		"" //do nothing
	);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Main(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, m_pOrbiter->m_sMainMenu, 
		/*StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", */
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Lights(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Media(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Climate(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Security(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Telephony(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Misc(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popLights(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popMedia(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popClimate(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popTelecom(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_popSecurity(long PK_Screen, string sLocation)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + "." + sLocation + ".0", 
		sLocation, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_SingleCameraViewOnly(long PK_Screen, string sPKDevice)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_CONST, sPKDevice);
	ScreenHandlerBase::SCREEN_SingleCameraViewOnly(PK_Screen, sPKDevice);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_QuadViewCameras(long PK_Screen, string sPKDevicesList)
{
	vector<string> vectDevices;
	StringUtils::Tokenize(sPKDevicesList, ",", vectDevices);

	if(vectDevices.size() == 4)
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, vectDevices[0]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, vectDevices[1]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_3_CONST, vectDevices[2]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_4_CONST, vectDevices[3]);
		ScreenHandlerBase::SCREEN_SingleCameraViewOnly(PK_Screen, sPKDevicesList);
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "The list of devices for SCREEN_QuadViewCameras is wrong: %s", 
			sPKDevicesList.c_str());
	}
}
//----------------------------------------------------------------------------------------------------- 
void ScreenHandler::SCREEN_NAS_Options(long PK_Screen, string sPK_DeviceTemplate, string sMacAddres, 
	string sIPAddress, string sPK_DHCPDevice)
{ 
	m_pOrbiter->m_pScreenHistory_NewEntry->m_sID = sMacAddres;
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sIPAddress);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sPK_DHCPDevice);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, sPK_DeviceTemplate);
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), "", "", false, false );
}
//----------------------------------------------------------------------------------------------------- 
//TODO: move the following code in OSDScreenHandler
//-----------------------------------------------------------------------------------------------------
// Users Wizard
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_UsersWizard(long PK_Screen)
{
	if(m_pWizardLogic->AlreadyHasUsers())
	{
		//skip "what's your name" screen
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_FamilyMembers_CONST), 
			"", "", false, false );
	}
	else
		ScreenHandlerBase::SCREEN_UsersWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::UsersWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, &ScreenHandler::UsersWizard_DatagridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::HandleAddUser()
{
	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	map<int, string>::iterator it = m_pOrbiter->m_mapVariable.find(VARIABLE_Seek_Value_CONST);
	string sUsername = it != m_pOrbiter->m_mapVariable.end() ? it->second : "";
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

	if(sUsername != "")
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
		m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), "", TEXT_USR_ENTRY_CONST);
		m_pWizardLogic->AddUser(sUsername);
	}
	else
	{
		m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), 
			"ERROR: Please enter your name", TEXT_USR_ENTRY_CONST);
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::UsersWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_YourName_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFamilyMembers_CONST)
			{
				if(HandleAddUser())
					return true;
			}
		}
		break;

		case DESIGNOBJ_FamilyMembers_CONST:
		{
			switch(pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butAdduser_CONST:
				{
					if(HandleAddUser())
						return true; //error message

					m_pOrbiter->CMD_Refresh("*");
				}
				break;

				case DESIGNOBJ_butRemoveUser_CONST:
				{
					int nPK_User = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
					PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
					if(nPK_User)
					{
						m_pWizardLogic->RemoveUser(nPK_User);
						m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), 
							"", TEXT_USR_ENTRY_CONST);
						m_pOrbiter->CMD_Refresh("*");
					}
					else
					{
						m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), 
							"ERROR: No user selected to be removed", TEXT_USR_ENTRY_CONST);
						return true;
					}
				}
				break;
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::UsersWizard_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_FamilyMembers_CONST:
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, pCellInfoData->m_sValue);
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Country Wizard
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CountryWizard(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_CountryWizard(PK_Screen);

	string sCountry = StringUtils::ltos(m_pWizardLogic->GetCountry());
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sCountry);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::CountryWizard_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::CountryWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Location_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butPostalCode_CONST)
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				int PK_Country = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
				m_pWizardLogic->SetCountry(PK_Country);

				string sCityRegion = m_pWizardLogic->GetCityRegion();
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sCityRegion);
			}
		}
		break;

		case DESIGNOBJ_PostalCode_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butWizRooms_CONST ) 
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sCityRegion = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];

				if(sCityRegion != "")
				{
					m_pWizardLogic->SetPostalCode(sCityRegion);
					int PK_Country = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, m_pWizardLogic->GetCityRegion());
					m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_PostalCode_CONST), 
						"", "", false, false);
					return true;
				}
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Rooms Wizard
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_RoomsWizard(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_RoomsWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::RoomsWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, &ScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, &ScreenHandler::RoomsWizard_DatagridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::RoomsWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Rooms_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butAddRoomType_CONST)
			{
				long PK_RoomType = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST].c_str());
				if(PK_RoomType)
				{
					m_pWizardLogic->AddRoomOfType(PK_RoomType);
					DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(StringUtils::ltos(DESIGNOBJ_dgRoomTypes_CONST));
					if(pObj)
					{
						PLUTO_SAFETY_LOCK(nd, m_pOrbiter->m_NeedRedrawVarMutex);
						m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObj);

						m_pOrbiter->InitializeGrid((DesignObj_DataGrid *)pObj);
						m_pOrbiter->RedrawObjects();
					}
				}
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butRemoveRoomType_CONST)
			{
				long PK_RoomType = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST].c_str());
				if(PK_RoomType)
				{
                    m_pWizardLogic->RemoveRoomOfType(PK_RoomType);
					DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(StringUtils::ltos(DESIGNOBJ_dgRoomTypes_CONST));
					if(pObj)
					{
						PLUTO_SAFETY_LOCK(nd, m_pOrbiter->m_NeedRedrawVarMutex);
						m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObj);

						m_pOrbiter->InitializeGrid((DesignObj_DataGrid *)pObj);
						m_pOrbiter->RedrawObjects();
					}
				}
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	switch(GetCurrentScreen_PK_DesignObj())	
	{
		case DESIGNOBJ_ConfirmNames_CONST:
		{
			int PK_Room = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
			string sName = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];

			if(sName != "" && PK_Room)
			{
				m_pWizardLogic->ChangeRoomName(PK_Room, sName);
				m_pOrbiter->CMD_Refresh("*");
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::RoomsWizard_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_ConfirmNames_CONST:
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, pCellInfoData->m_sValue);
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, pCellInfoData->m_sText);
			m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), 
				pCellInfoData->m_sText, TEXT_USR_ENTRY_CONST);
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// TV Provider Wizard
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_TV_provider(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_TV_provider(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::TV_provider_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::TV_provider_ObjectSelected(CallBackData *pData)
{
g_pPlutoLogger->Write(LV_CRITICAL, "Variable 12 = %s", m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());

	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVProvider_CONST:
		{
			if(DESIGNOBJ_butTVBoxManuf_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput1 = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				string sInput2 = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];

				if(sInput1 != "")
				{
					DCE::SCREEN_TV_Manufacturer SCREEN_TV_Manufacturer_(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device);
					string sResult;
					m_pOrbiter->CMD_Goto_Screen("", SCREEN_TV_Manufacturer_CONST, sResult, SCREEN_TV_Manufacturer_.m_pMessage);
					return true;
				}
				else if(sInput2 != "")
				{
					//do nothing.. it will go to the next screen
				}
				else
					return true;
			}
		}
		break;
			

		case DESIGNOBJ_TVBoxManuf_CONST:
		{
			if(DESIGNOBJ_butTVBoxInput_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_TVBoxModel_CONST:
		{
			if(DESIGNOBJ_butTVManuf_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sModel == "")
					return true;

				//we have the TVBox Manufacturer and TVBox Model!
				m_nPK_Device_TVProvider_External = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// A/V Connection Wizard
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_TV_Manufacturer(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_TV_Manufacturer(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::TV_Manufacturer_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::TV_Manufacturer_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVManuf_CONST:
		{
			if(DESIGNOBJ_butTVModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_TVModel_CONST:
		{
			if(DESIGNOBJ_butReceiver_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sModel == "")
					return true;

				//we have the TV Manufacturer and TV Model!
				long nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));

				if(m_nPK_Device_TVProvider_External)
				{
					m_pWizardLogic->SetAvPath(m_nPK_Device_TVProvider_External, nPK_Device_TV, 
						0 /*audio and video*/, COMMAND_Tuner_CONST);
				}
				else
				{
					long nPK_Device_Generic_Tuner = m_pWizardLogic->AddDevice(DEVICETEMPLATE_Generic_Tuner_CONST); 
					m_pWizardLogic->SetAvPath(nPK_Device_Generic_Tuner, nPK_Device_TV, 
						0 /*audio and video*/, COMMAND_Tuner_CONST);
				}
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Receiver
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Receiver(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Receiver(PK_Screen);

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");

	RegisterCallBack(cbObjectSelected, &ScreenHandler::Receiver_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Receiver_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_ReceiverManuf_CONST:
		{
			if(DESIGNOBJ_butReceiverModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_ReceiverModel_CONST:
		{
			if(DESIGNOBJ_butReceiverInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sModel == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_ReceiverInputs_CONST:
			{
				if(DESIGNOBJ_butAVDevices_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
				{
					string sReceiverInputs = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
					if(sReceiverInputs == "")
						return true;

					string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
					int PK_Receiver = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));

					int PK_Device_From = 0;
					int PK_Command_Input = atoi(sReceiverInputs.c_str());
					
					//todo
					//is this correct? I think we need here an extra step, to get to output
					//m_pWizardLogic->SetAvPath(PK_Device_From, PK_Receiver, 1 /*audio*/, PK_Command_Input);
					//m_pWizardLogic->SetAvPath(PK_Device_From, PK_Receiver, 2 /*video*/, PK_Command_Input);
				}
			}
			break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Other AV Devices
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_AV_Devices(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");

	ScreenHandlerBase::SCREEN_AV_Devices(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::AV_Devices_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AV_Devices_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AVDevices_CONST:
		{
			if(DESIGNOBJ_butAVDevicesManuf_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sDeviceType = m_pOrbiter->m_mapVariable[VARIABLE_Datagrid_Filter_CONST];
				if(sDeviceType == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_AVDeviceManuf_CONST:
		{
			if(DESIGNOBJ_butAVDeviceModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sManufacturer == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_AVDeviceModel_CONST:
		{
			if(DESIGNOBJ_butAVOutputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sModel == "")
					return true;

				//we have the AV Device Manufacturer and AV Device Model!
				m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_LightsSetup(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_LightsSetup(PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_AlarmPanel(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_AlarmPanel(PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
// VOIP Provider
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_VOIP_Provider(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");

	ScreenHandlerBase::SCREEN_VOIP_Provider(PK_Screen);

	RegisterCallBack(cbObjectSelected, &ScreenHandler::VOIP_Provider_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::VOIP_Provider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{

		case DESIGNOBJ_VoipProvider_CONST:
		{
			if(DESIGNOBJ_butConfigureAccount_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sVOIPProvider = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
				if(sVOIPProvider == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_VoipNumber_CONST:
		{
			if(DESIGNOBJ_butVoipUsername_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sPhoneNumber = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPhoneNumber);
				if(sPhoneNumber == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_VoipUsername_CONST:
		{
			if(DESIGNOBJ_butVoipPassword_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sUsername = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sUsername);
				if(sUsername == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_VoipPassword_CONST:
		{
			if(DESIGNOBJ_butFinal_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sPassword = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
				if(sPassword == "")
					return true;

				string sPhoneNumber = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				string sUsername = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST];
				string sVOIPProvider = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];

				DCE::CMD_PL_Add_VOIP_Account CMD_PL_Add_VOIP_Account_(m_pOrbiter->m_dwPK_Device, 
					m_pOrbiter->m_dwPK_Device_TelecomPlugIn, sVOIPProvider, sPhoneNumber, sUsername, sPassword);
				m_pOrbiter->SendCommand(CMD_PL_Add_VOIP_Account_);
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
