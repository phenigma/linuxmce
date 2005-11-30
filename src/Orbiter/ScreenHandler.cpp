#include "ScreenHandler.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : ScreenHandlerBase(p_MapDesignObj)
{
	m_pOrbiter = pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
ScreenHandler::~ScreenHandler() 
{
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
bool ScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	g_pPlutoLogger->Write(LV_WARNING, "FileSave_ObjectSelected executed!");
	return true;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileSave(long PK_Screen, string sPrivate, string sPublic, string sCaption)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPrivate);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sPublic);
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sCaption, TEXT_STATUS_CONST);
	ScreenHandlerBase::SCREEN_FileSave(PK_Screen, sPrivate, sPublic, sCaption);

	//registering callbacks - sample
	RegisterCallBack(cbObjectSelected, FileSave_ObjectSelected, new CallBackData());
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
	DisplayMessageOnOrbiter(PK_Screen, sDescription, false, "0", sCannotGoBack == "1");
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

