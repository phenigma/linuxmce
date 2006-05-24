#if (USE_WX_LIB)
#include "../wxAppMain/wx_all_include.cpp"
#include "../wxAppMain/wx_safe_dialog.h"
#include "../wxAppMain/wxdialog_roomwizard.h"
#define USE_TASK_MANAGER true
#endif // (USE_WX_LIB)

#include "../Task.h"
#include "../TaskManager.h"

#ifndef WIN32
#include "OrbiterLinux.h"
#endif

#include "OSDScreenHandler.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_CommMethod.h"
#include "pluto_main/Define_Country.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "../WizardLogic.h"
#include "pluto_main/Define_FloorplanObjectType.h"
#include "DataGrid.h"

#ifdef ENABLE_MOUSE_BEHAVIOR
#	include "../MouseBehavior.h"
#	include "../SpeedMouseHandler.h"
#	include "../VolumeMouseHandler.h"
#	include "../LightMouseHandler.h"
#endif

//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
        ScreenHandler(pOrbiter, p_MapDesignObj)
{
	m_bLightsFlashThreadRunning=m_bLightsFlashThreadQuit=false;
	m_pWizardLogic = new WizardLogic(pOrbiter);
	m_dwMessageInterceptorCounter_ReportingChildDevices = 0;
	if(!m_pWizardLogic->Setup())
	{
		//do something here; unable to connect to db
	}
}
//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::~OSDScreenHandler()
{
	delete m_pWizardLogic;
}

//-----------------------------------------------------------------------------------------------------
// Video Wizard greetings screen
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_VideoWizard(long PK_Screen)
{
m_pOrbiter->m_bNewOrbiter=true;
	if( m_pWizardLogic->HouseAlreadySetup() && !m_pOrbiter->m_bNewOrbiter )
	{
		if( m_pWizardLogic->HasRemoteControl() )
			// Everthing is setup already.  Goto the 'pick a wizard screen'
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_Which_Wizard_CONST);
		else
		{
			// Help the user get a remote control
			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_NoRemoteControl_CONST),
										"", "", false, false );
			m_pOrbiter->StartScreenHandlerTimer(500);
		}
	}
	else
		ScreenHandlerBase::SCREEN_VideoWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnTimer,	(ScreenHandlerCallBack) &OSDScreenHandler::VideoWizard_OnTimer, new CallBackData());
}

bool OSDScreenHandler::VideoWizard_OnTimer(CallBackData *pData)
{
	if(m_pWizardLogic->HasRemoteControl(false))
	{
		NeedToRender render2( m_pOrbiter, "OSDScreenHandler::VideoWizard_OnTimer" );  // Redraw anything that was changed by this command
		if( !m_pWizardLogic->HouseAlreadySetup() )
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
		else
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_Which_Wizard_CONST);
		return false;
	}
	return true;
}

bool OSDScreenHandler::VideoWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Greetings_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butType_of_Wizard_CONST)
			{
				if( !m_pWizardLogic->HasRemoteControl() )
				{
					// Help the user get a remote control
					m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_NoRemoteControl_CONST),
												"", "", false, false );
					m_pOrbiter->StartScreenHandlerTimer(500);
					return true;
				}
				else if( !m_pWizardLogic->HouseAlreadySetup() )
				{
					m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
					return true;
				}
			}
			break;
		}
		case DESIGNOBJ_NoRemoteControl_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butNoRemote_TypeOfWizard_CONST)
			{
				if( !m_pWizardLogic->HouseAlreadySetup() )
				{
					m_pOrbiter->CMD_Goto_Screen("",SCREEN_UsersWizard_CONST);
					return true;
				}
			}
		}
	};
	return false;
}

//-----------------------------------------------------------------------------------------------------
// Users Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_UsersWizard(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	if(m_pWizardLogic->AlreadyHasUsers())
	{
		//skip "what's your name" screen
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_FamilyMembers_CONST),
                                       "", "", false, false );
	}
	else
		ScreenHandlerBase::SCREEN_UsersWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::UsersWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::UsersWizard_DatagridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::HandleAddUser(bool bErrorIfEmpty)
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
	else if( bErrorIfEmpty )
	{
		m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                 "ERROR: Please enter your name", TEXT_USR_ENTRY_CONST);
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::UsersWizard_ObjectSelected(CallBackData *pData)
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

				case DESIGNOBJ_butLocation_CONST:
				{
					HandleAddUser(false);
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
bool OSDScreenHandler::UsersWizard_DatagridSelected(CallBackData *pData)
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
void OSDScreenHandler::SCREEN_CountryWizard(long PK_Screen)
{
	if( m_pWizardLogic->GetLocation() )
	{
		PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
		if( m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_5_CONST]!="VALID_DATA_FOUND" )  // A keyword so we know when we already asked this question
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, "VALID_DATA_FOUND");
			DisplayMessageOnOrbiter(SCREEN_PopupMessage_CONST,"Is this your location?\n<%=31%>",false,"0",false,"Yes","0 -300 1 741 159 192","No","0 -300 1 741 159 194");
			return;
		}
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_CountryWizard(PK_Screen);

	string sCountry = StringUtils::ltos(m_pWizardLogic->GetCountry());
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sCountry);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::CountryWizard_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::CountryWizard_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);

	if(NULL == pObjectInfoData)
		return false;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Location_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butPostalCode_CONST) // 4661
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				int PK_Country = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
				m_pWizardLogic->SetCountry(PK_Country);

				string sCityRegion = m_pWizardLogic->GetCityRegion();
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sCityRegion);
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_PostalCode_CONST),TEXT_STATUS_CONST );
				if( pText )
				{
					if( PK_Country==COUNTRY_UNITED_STATES_CONST )  // Actually you can search for postal code no matter the country, but so far they're only populated for the U.S., and we call them 'Zip Codes'
					{
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Enter_your_postal_code_CONST]; // 1593
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,"enterpostal");
					}
					else
					{
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_postal_code_CONST];  // 1557
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,"entercity");
					}
				}
			}
		}
		break;

		case DESIGNOBJ_PostalCode_CONST: // 4662
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butWizRooms_CONST )
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sLocation = m_pOrbiter->m_mapVariable[VARIABLE_Datagrid_Input_CONST];
				bool bLocationOk = m_pWizardLogic->SetLocation(sLocation);
				if( !bLocationOk )
				{
					DisplayMessageOnOrbiter(SCREEN_PopupMessage_CONST,"location isn't good",false,"0",false,"Try again","0 -300 1 741 159 194","Skip it","0 -300 1 741 159 192");
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
void OSDScreenHandler::SCREEN_RoomsWizard(long PK_Screen)
{
/*
#if (USE_WX_LIB)
    {
        ScreenHandler::SCREEN_RoomsWizard(PK_Screen);
        // register the RoomWizard callbacks
        g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::SCREEN_RoomsWizard()" );
        RegisterCallBack( cbOnDialogCreate, (ScreenHandlerCallBack)&OSDScreenHandler::RoomsWizardCreate, new PositionCallBackData() );
        RegisterCallBack( cbOnDialogDelete, (ScreenHandlerCallBack)&OSDScreenHandler::RoomsWizardDelete, new PositionCallBackData() );
        RegisterCallBack( cbOnDialogRefresh, (ScreenHandlerCallBack)&OSDScreenHandler::RoomsWizardRefresh, new PositionCallBackData() );
        return;
    } // USE_WX_LIB
#endif
*/

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_RoomsWizard(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::RoomsWizard_DatagridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizard_ObjectSelected(CallBackData *pData)
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
					RefreshDatagrid(DESIGNOBJ_dgRoomTypes_CONST);
				}
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butRemoveRoomType_CONST)
			{
				long PK_RoomType = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST].c_str());
				if(PK_RoomType)
				{
                    m_pWizardLogic->RemoveRoomOfType(PK_RoomType);
					RefreshDatagrid(DESIGNOBJ_dgRoomTypes_CONST);
				}
			}
		}
		break;

		case DESIGNOBJ_ConfirmNames_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVProvider_CONST)
			{
				if(!m_pWizardLogic->WhatRoomIsThisDeviceIn(m_pOrbiter->m_dwPK_Device))
				{
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
					//the user removed the initial room while picking the room types; redirecting
					m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_InWhichRoomIsTheSystem_CONST),
                                                   "", "", false, false);
					return true;
				}
			}
		}
		break;

		case DESIGNOBJ_InWhichRoomIsTheSystem_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVProvider_CONST)
			{
				string sPK_Room = m_pOrbiter->m_mapVariable[VARIABLE_Datagrid_Input_CONST];
				if(sPK_Room == "")
					return true;

				int nPK_Device_TopMost = m_pWizardLogic->GetTopMostDevice(m_pOrbiter->m_dwPK_Device);
				g_pPlutoLogger->Write(LV_WARNING, "Setting the room for top most device %d, room %s",
                                      nPK_Device_TopMost, sPK_Room.c_str());
				m_pWizardLogic->SetRoomForDevice(StringUtils::ltos(nPK_Device_TopMost), sPK_Room);
				m_pOrbiter->m_pData->m_dwPK_Room = atoi(sPK_Room.c_str());
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizard_CapturedKeyboardBufferChanged(CallBackData *pData)
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
bool OSDScreenHandler::RoomsWizard_DatagridSelected(CallBackData *pData)
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
void OSDScreenHandler::SCREEN_TV_provider(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "0");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "0");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Location_CONST, StringUtils::itos(m_pWizardLogic->GetPostalCode()));

	ScreenHandlerBase::SCREEN_TV_provider(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_provider_ObjectSelected, 
		new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_provider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVProvider_CONST:
		{
			if(DESIGNOBJ_butTVBoxManuf_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput1 = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				string sInput2 = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];

				int PK_PostalCode = m_pWizardLogic->GetPostalCode();
				DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_LocalAppServer,
                                                                 "/usr/pluto/bin/SetupMythHack.sh","mythhack",StringUtils::itos(PK_PostalCode),"","",false,false,false);
				m_pOrbiter->SendCommand(CMD_Spawn_Application);

				if(atoi(sInput1.c_str())==0)
				{
					DCE::SCREEN_TV_Manufacturer SCREEN_TV_Manufacturer_(m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device);
					string sResult;
					m_pOrbiter->CMD_Goto_Screen("", SCREEN_TV_Manufacturer_CONST, sResult, SCREEN_TV_Manufacturer_.m_pMessage);
					return true;
				}

				// Temporary hack until we get a solution for auto-configuring Myth
				return false;
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
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_Cable_Boxes_CONST);
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_Satellite_Boxes_CONST);
				m_pWizardLogic->m_nPK_Device_TVProvider_External = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				m_pWizardLogic->AddExternalTuner(m_pWizardLogic->m_nPK_Device_TVProvider_External);  // Make this point to our PVR card's input
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// A/V Connection Wizard
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_TV_Manufacturer(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_TV_Manufacturer(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_GridSelected, 
		new DatagridCellBackData());

	RegisterCallBack(cbCapturedKeyboardBufferChanged, 
		(ScreenHandlerCallBack) &OSDScreenHandler::AV_Devices_CapturedKeyboardBufferChanged,
		new ObjectInfoBackData());

}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_Manufacturer_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Available_Serial_Ports_CONST )
	{
		string::size_type pos=0;
		int iPK_Device_ControlledVia = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,",",pos).c_str());
		string sPort = StringUtils::Tokenize(pCellInfoData->m_sValue,",",pos);

		string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
		if(sModel == "")
			return true;

		m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST);
		m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()),
                                                                    StringUtils::itos(DEVICEDATA_COM_Port_on_PC_CONST) + "|" + sPort,
                                                                    iPK_Device_ControlledVia);
		m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_DirectToTV_CONST),"","",true,false);
		return true;
	}
	
	//Video Wizard 	
	DesignObj_Orbiter *pObj = NULL;
	DesignObj_DataGrid *pDatagridObj = NULL;
	DataGridCell *pCell = NULL;
	string sVal, sId, sAux;
	int nId;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVManuf_CONST:
			m_pWizardLogic->SetManufacurerID( atoi(pCellInfoData->m_sValue.c_str() ) );
			m_pWizardLogic->SetManufacturer( pCellInfoData->m_sText.c_str() );
		break;

		case DESIGNOBJ_TVModel_CONST:
			sAux = pCellInfoData->m_sText;
			sAux = pCellInfoData->m_sValue;
		break;

		case DESIGNOBJ_TVWhatDelays_CONST:
		pObj = m_pOrbiter->FindObject(DESIGNOBJ_dgTVWhatDelays_CONST);

		pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);
		if(NULL != pCellInfoData)
			sId = pCellInfoData->m_sValue;
		sVal = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
		nId = atoi( sId.c_str() );
		if( NULL != pObj )
		{
			pDatagridObj = dynamic_cast<DesignObj_DataGrid *>(pObj);

			if(NULL != pDatagridObj)
			{
				pCell = pDatagridObj->m_pDataGridTable->GetData(1, nId);
				if( NULL != pCell )
				{
					sVal = pCell->GetText();
					//PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
					m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), sVal, TEXT_USR_ENTRY_CONST);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sVal);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sVal);
					//m_pOrbiter->CMD_Refresh("*");
				}
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_Manufacturer_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	int nDeviceId = 0;
	string aux;

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		//Start wizard
		case DESIGNOBJ_TVManufNotListed_CONST:
		m_pWizardLogic->SetDeviceCategory(77);

		if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVModelNotListed_CONST )
		{
			PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
			map<int, string>::iterator it = m_pOrbiter->m_mapVariable.find(VARIABLE_Seek_Value_CONST);
			string manuf = it != m_pOrbiter->m_mapVariable.end() ? it->second : "";
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

			if( manuf.empty() )
			{
				m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                 "Please specify a name for Manufacturer ", TEXT_STATUS_CONST);
                return true;
			}
			
			if( m_pWizardLogic->ExistManufacturer(manuf) )
			{
				m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                 "Manufacturer " + manuf + " already exist", TEXT_STATUS_CONST);
				return true;
			}
			else
				m_pWizardLogic->SetManufacturer( manuf );
		}
		break;

		case DESIGNOBJ_TVModelNotListed_CONST:
		if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVWhatDelays_CONST )
		{
			PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
			map<int, string>::iterator it = m_pOrbiter->m_mapVariable.find(VARIABLE_Seek_Value_CONST);
			string tvModel = it != m_pOrbiter->m_mapVariable.end() ? it->second : "";
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

			if( tvModel == "" )
			{
				m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                 "Please specify a model", TEXT_STATUS_CONST);
                return true;
			}
			m_pWizardLogic->SetAVTemplateName( tvModel );
			int nPK = m_pWizardLogic->AddAVDeviceTemplate();
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::itos( nPK ));
		}
		break;

		case DESIGNOBJ_TVWhatDelays_CONST:
		if( (pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVHowToTune_CONST) )
		{
			//reset edit
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
			m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                     "", TEXT_USR_ENTRY_CONST);
        }
		break;

		case DESIGNOBJ_TVHowToTune_CONST:
				switch( pObjectInfoData->m_PK_DesignObj_SelectedObject )
				{
				case DESIGNOBJ_butHowToTuneYes_CONST:
					m_pWizardLogic->SetAVTemplateTogglePower( true );
				break;

				case DESIGNOBJ_butHowToTuneNo_CONST:
					m_pWizardLogic->SetAVTemplateTogglePower( false );
				break;

				case DESIGNOBJ_butSQLOnOffCodes_CONST:
					m_pWizardLogic->UpdateAVTemplateSettings();
					CMD_Create_Device cmd( m_pOrbiter->m_dwPK_Device,
						m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn,m_pWizardLogic->GetAVTemplateId(),
						"",0,"","",0,DEVICETEMPLATE_gc100_CONST,
						m_pOrbiter->m_dwPK_Device,0,&nDeviceId);
					m_pOrbiter->SendCommand(cmd);
					m_pWizardLogic->SetDeviceId(nDeviceId) ;
					m_pWizardLogic->GetParentDevice();
					switch( m_pWizardLogic->GetAVIRCodesType() )
					{
						//Unknow
						case 0:
						m_pOrbiter->CMD_Goto_Screen( "", SCREEN_TVOnOffCodes_CONST );
						break;

						//Toggle
						case 1:
						m_pOrbiter->CMD_Goto_Screen( "", SCREEN_OnOff_Toggle_CONST );
						break;

						//Discret
						case 2:
						m_pOrbiter->CMD_Goto_Screen( "", SCREEN_TVConfirmOnOffDiscre_CONST );
						break;
					}	
				return false;
				break;
				}
		break;
		//End wizard

		case DESIGNOBJ_TVManuf_CONST:
		{
			if(DESIGNOBJ_butTVModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sManufacturer = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sManufacturer == "")
					return true;

				m_pWizardLogic->SetDeviceCategory(77); 
				m_pWizardLogic->SetAVTemplateID( atoi(sManufacturer.c_str()) );
			}
		}
		break;

		case DESIGNOBJ_TVModel_CONST:
		{
			if(DESIGNOBJ_butDirectToTv_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sModel == "")
					return true;

				// Delete tv's first since maybe the user is returning to this wizard
				int PK_CommMethod = DatabaseUtils::GetCommMethodForDeviceTemplate(m_pWizardLogic,atoi(sModel.c_str()));
				if( PK_CommMethod==COMMMETHOD_RS232_CONST )
				{
					m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_SelectPort_CONST),"","",false,false);
					return true;  // We're redirecting the flow
				}
				else
				{
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST);
					m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				}
			}
		}
		break;

		case DESIGNOBJ_DirectToTV_CONST:
		{
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_WhichTVInput_CONST),TEXT_STATUS_CONST );
			if(DESIGNOBJ_butDirectlyToTv_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->m_bUsingReceiverForVideo = false;
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_input_from_md_CONST];
			}
			else if(DESIGNOBJ_butToReceiver_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pWizardLogic->m_bUsingReceiverForVideo = true;
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_input_from_Receiver_to_TV_CONST];
			}
		}
		break;

		case DESIGNOBJ_WhichTVInput_CONST:
		{
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butReceiver_CONST )
			{
				m_pWizardLogic->m_nPK_Command_Input_Video_On_TV = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST].c_str());
				if( !m_pWizardLogic->m_bUsingReceiverForVideo )
					m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_TV,2,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV); // 2 = video
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Receiver
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_Receiver(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	if( m_pWizardLogic->m_bUsingReceiverForVideo )
		m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_ReceiverManuf_CONST), "", "", false, false );
	else
		ScreenHandlerBase::SCREEN_Receiver(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::Receiver_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::Receiver_GridSelected, new DatagridCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Available_Serial_Ports_CONST )
	{
		string::size_type pos=0;
		int iPK_Device_ControlledVia = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,",",pos).c_str());
		string sPort = StringUtils::Tokenize(pCellInfoData->m_sValue,",",pos);

		string sModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
		if(sModel == "")
			return true;

		// Delete receivers first since maybe the user is returning to this wizard
		m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
		m_pWizardLogic->m_nPK_Device_Receiver = m_pWizardLogic->AddDevice(atoi(sModel.c_str()),
                                                                          StringUtils::itos(DEVICEDATA_COM_Port_on_PC_CONST) + "|" + sPort,
                                                                          iPK_Device_ControlledVia);

		m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_ReceiverInputs_CONST),"","",true,false);
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::Receiver_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Receiver_CONST:
		{
			if(DESIGNOBJ_butTV_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				// No Receiver -- I use the tv for audio
				m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                          m_pWizardLogic->m_nPK_Device_TV,1,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV); // 1 = audio
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
			}
		}
		break;

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

				int PK_CommMethod = DatabaseUtils::GetCommMethodForDeviceTemplate(m_pWizardLogic,atoi(sModel.c_str()));
				if( PK_CommMethod==COMMMETHOD_RS232_CONST )
				{
					m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_SelectPort_CONST),"","",false,false);
					return true;  // We're redirecting the flow
				}
				else
				{
					// Delete receivers first since maybe the user is returning to this wizard
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
					m_pWizardLogic->m_nPK_Device_Receiver = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				}
			}
		}
		break;

		case DESIGNOBJ_SelectPort_CONST:
		{
			if(DESIGNOBJ_butAlarmDetect_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_ReceiverInputs_CONST),"","",false,false);
				return true;  // We're redirecting to normal flow (DESIGNOBJ_SelectPort_CONST IS USED ON ALARM PANEL!!!)
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

                //int PK_Device_From = 0;
                int PK_Command_Input = atoi(sReceiverInputs.c_str());

                // Set the input
                m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                          m_pWizardLogic->m_nPK_Device_Receiver,1 /* audio */,PK_Command_Input);

                // Video also goes to the receiver, and the receiver's video goes to the tv
                if( m_pWizardLogic->m_bUsingReceiverForVideo )
                {
                    m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
                                              m_pWizardLogic->m_nPK_Device_Receiver,2 /* video */,PK_Command_Input);
                    m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Receiver,
                                              m_pWizardLogic->m_nPK_Device_TV,2 /* video */,m_pWizardLogic->m_nPK_Command_Input_Video_On_TV);
                }
            }
        }
        break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// Other AV Devices
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_AV_Devices(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_AV_Devices(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AV_Devices_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------

bool OSDScreenHandler::AV_Devices_ObjectSelected(CallBackData *pData)
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
				m_pWizardLogic->m_nPK_Device_Last_AV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
			}
		}
		break;

		case DESIGNOBJ_AVDeviceVideo_CONST:
		{
			if(DESIGNOBJ_butAVDeviceVideoInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInstalledAVDevice = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sInstalledAVDevice == "")
					return true;

				m_pWizardLogic->m_nPK_Device_AV_Installed = atoi(sInstalledAVDevice.c_str());
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,
                                             StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AV_Installed)));
			}
		}
		break;

		case DESIGNOBJ_AVDeviceVideoInputs_CONST:
		{
			if(DESIGNOBJ_butAVDeviceAudio_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
				if(sInput != "")
				{
					m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
                                              m_pWizardLogic->m_nPK_Device_AV_Installed, 2 /* video */, atoi(sInput.c_str()));
				}
			}
		}
		break;

		case DESIGNOBJ_AVDeviceAudio_CONST:
		{
			if(DESIGNOBJ_butAVDeviceAudioInputs_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInstalledAVDevice = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sInstalledAVDevice == "")
					return true;

				m_pWizardLogic->m_nPK_Device_AV_Installed = atoi(sInstalledAVDevice.c_str());
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,
                                             StringUtils::itos(DatabaseUtils::GetDeviceTemplateForDevice(m_pWizardLogic,m_pWizardLogic->m_nPK_Device_AV_Installed)));
			}
		}
		break;

		case DESIGNOBJ_AVDeviceAudioInputs_CONST:
		{
			if(DESIGNOBJ_butMoreAVDevices_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sInput = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
				if(sInput != "")
				{
					m_pWizardLogic->SetAvPath(m_pWizardLogic->m_nPK_Device_Last_AV,
                                              m_pWizardLogic->m_nPK_Device_AV_Installed, 1 /* audio */, atoi(sInput.c_str()));
				}
			}
		}
		break;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_LightsSetup(long PK_Screen)
{
	m_pWizardLogic->LookForZWave();
	if( !m_pWizardLogic->m_nPK_Device_ZWave )
	{
		// TODO: This is monster specific
		// Just go to a 'add other devices' design obj
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
	}
	m_nLightInDequeToAssign=0;

	ScreenHandlerBase::SCREEN_LightsSetup(PK_Screen);
	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_Intercepted, new MsgInterceptorCellBackData());
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_SelectedGrid, new DatagridCellBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_OnScreen, new RenderScreenCallBackData());
	RegisterCallBack(cbOnGotoScreen, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_OnGotoScreen, new GotoScreenCallBackData());
}
//-----------------------------------------------------------------------------------------------------

// TODO: This is Monster specific
bool OSDScreenHandler::LightsSetup_OnGotoScreen(CallBackData *pData)
{
	//GotoScreenCallBackData *pGotoScreenCallBackData = (GotoScreenCallBackData *)pData;
	m_bLightsFlashThreadQuit=true;
	return false;
}

void *LightsFlashThread(void *p)
{
	OSDScreenHandler* pOSDScreenHandler = (OSDScreenHandler *)p;
	pOSDScreenHandler->LightsSetup_Timer();
	return NULL;
}

bool OSDScreenHandler::LightsSetup_OnScreen(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( pRenderScreenCallBackData->m_pObj->m_iBaseObjectID==DESIGNOBJ_LightSetupRooms_CONST )
	{
		if( m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
		{
			PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex);
			if( !m_bLightsFlashThreadRunning )
			{
				m_bLightsFlashThreadRunning=true;
				m_bLightsFlashThreadQuit=false;
				pthread_t t;
				pthread_create(&t, NULL, LightsFlashThread, (void*)this);
			}
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
			m_pOrbiter->CMD_Refresh("");
		}
	}
	return false;
}

void OSDScreenHandler::LightsSetup_Timer()
{
	// We're going to create our own sockets since we're looking for delivery confirmation on
	// the flashing commands so we can report status to the user.  These may be slow, so do
	// this on it's own socket so it doesn't block sending/receiving datagrads and other ui stuff
	Event_Impl event_Impl(m_pOrbiter->m_dwPK_Device, 0, m_pOrbiter->m_sHostName);

	static bool bLastTimeOn=true;
	while( !m_bLightsFlashThreadQuit && m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
	{
		PLUTO_SAFETY_LOCK(vm,m_pOrbiter->m_VariableMutex);
		string sID = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].second;
		int PK_Device = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first;
		string sDescription = DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,PK_Device);

		string sText = string("Turning ") + (bLastTimeOn ? "OFF " : "ON ") + StringUtils::itos(PK_Device) + " " + sDescription + "...";
		DesignObj_Orbiter *pObjStatus = m_pOrbiter->FindObject(StringUtils::itos(DESIGNOBJ_LightSetupRooms_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_objLightStatus_CONST));
		DesignObjText *pText = m_pOrbiter->FindText( pObjStatus,TEXT_STATUS_CONST );
		if( pText )
			pText->m_sText = sText;
		vm.Release();

		{
			if(NULL != pObjStatus)
				m_pOrbiter->RenderObjectAsync(pObjStatus);
			/*
              PLUTO_SAFETY_LOCK( nd, m_pOrbiter->m_NeedRedrawVarMutex );
              if( pObjStatus )
              m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObjStatus);
              nd.Release();
			*/
			NeedToRender render2( m_pOrbiter, "OSDScreenHandler::LightsSetup_Timer1" );  // Redraw anything that was changed by this command
		}

		string sResponse;
		if( !bLastTimeOn )
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave, // Monster specific
                                                                     sID,COMMAND_Generic_On_CONST,"");
			event_Impl.SendMessage(CMD_Send_Command_To_Child.m_pMessage,sResponse);
		}
		else
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave, // Monster specific
                                                                     sID,COMMAND_Generic_Off_CONST,"");
			event_Impl.SendMessage(CMD_Send_Command_To_Child.m_pMessage,sResponse);
		}
		bLastTimeOn=!bLastTimeOn;

		vm.Relock();
		if( pText )
			pText->m_sText = sText + "(" + sResponse  + ")";
		vm.Release();

		{
			if(NULL != pObjStatus)
				m_pOrbiter->RenderObjectAsync(pObjStatus);
			/*
              PLUTO_SAFETY_LOCK( nd, m_pOrbiter->m_NeedRedrawVarMutex );
              if( pObjStatus )
              m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObjStatus);
			*/
			NeedToRender render2( m_pOrbiter, "OSDScreenHandler::LightsSetup_Timer2" );  // Redraw anything that was changed by this command
		}

		time_t timeout = time(NULL) + 4;
		int nLightInDequeToAssign=m_nLightInDequeToAssign;
		while(timeout>time(NULL) && nLightInDequeToAssign==m_nLightInDequeToAssign && !m_bLightsFlashThreadQuit)
			Sleep(500);  // max 500 ms delay
	}
	m_bLightsFlashThreadRunning=false;
}

bool OSDScreenHandler::LightsSetup_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_Lights_CONST:
		{
			if(DESIGNOBJ_butIHaveMonster_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				if( !m_dwMessageInterceptorCounter_ReportingChildDevices )
				{
					m_dwMessageInterceptorCounter_ReportingChildDevices = m_pOrbiter->RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter::ScreenHandlerMsgInterceptor),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Reporting_Child_Devices_CONST);
					m_pOrbiter->RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter::ScreenHandlerMsgInterceptor),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Download_Config_Done_CONST);
				}
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_LightsSetup_CONST),TEXT_STATUS_CONST );
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Lights_Setup_CONST];
			}
		}
		break;

		case DESIGNOBJ_LightsPrepare_CONST:
		{
			if(DESIGNOBJ_butMonsterLightsSetup_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sResponse;
				DCE::CMD_Download_Configuration CMD_Download_Configuration(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave,"");
				if( !m_pOrbiter->SendCommand(CMD_Download_Configuration,&sResponse) )
				{
					DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_LightsSetup_CONST),TEXT_STATUS_CONST );
					if( pText )
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": ZWave interface not responding";
				}
			}
		}
		break;
	};

	return false;
}

bool OSDScreenHandler::LightsSetup_SelectedGrid(CallBackData *pData)
{
	DatagridCellBackData *pDatagridCellBackData = (DatagridCellBackData *) pData;
	if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_Rooms_CONST )
		if( !DatabaseUtils::SetDeviceInRoom(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,atoi(pDatagridCellBackData->m_sValue.c_str())) )
			return true;  // This room isn't valid
		else
			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_LightType_CONST),"","",false,false);
	else if( pDatagridCellBackData->m_nPK_Datagrid == DATAGRID_LightType_CONST )
	{
		DatabaseUtils::SetDeviceData(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first,
                                     DEVICEDATA_PK_FloorplanObjectType_CONST,pDatagridCellBackData->m_sValue);
		m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_LightSetupRooms_CONST),"","",false,false);
		m_nLightInDequeToAssign++;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::LightsSetup_Intercepted(CallBackData *pData)
{
	if(GetCurrentScreen_PK_DesignObj()==DESIGNOBJ_LightsSetup_CONST)
	{
		MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;
		if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT &&
            pMsgInterceptorCellBackData->m_pMessage->m_dwID==EVENT_Download_Config_Done_CONST )
		{
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_LightsSetup_CONST),TEXT_STATUS_CONST );
			string sResult = pMsgInterceptorCellBackData->m_pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];

			if( sResult.size() )
			{
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": " + sResult;
				m_pOrbiter->CMD_Refresh("");
				return false;
			}
			if( pText )
				pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Monster_Remote_Found_CONST];

			DCE::CMD_Report_Child_Devices CMD_Report_Child_Devices(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave);
			m_pOrbiter->SendCommand(CMD_Report_Child_Devices);
			m_pOrbiter->CMD_Refresh("");
			return false;
		}
		if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_EVENT &&
            pMsgInterceptorCellBackData->m_pMessage->m_dwID==EVENT_Reporting_Child_Devices_CONST )
		{
			string sResult = pMsgInterceptorCellBackData->m_pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
			if( sResult.size() )
			{
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_LightsSetup_CONST),TEXT_STATUS_CONST );
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Error_getting_data_CONST] + ": " + sResult;
				m_pOrbiter->CMD_Refresh("");
				return false;
			}

			int iNumLightsTotal = m_pWizardLogic->GetNumLights(iNumLightsTotal);

			if( iNumLightsTotal==0 )
			{
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_LightsSetup_CONST),TEXT_STATUS_CONST );
				if( pText )
					pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_No_Lights_Found_CONST] + ": " + sResult;
				m_pOrbiter->CMD_Refresh("");
				return false;
			}

			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,StringUtils::itos(iNumLightsTotal));
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,StringUtils::itos((int) m_pWizardLogic->m_dequeNumLights.size()));

			if( m_pWizardLogic->m_dequeNumLights.size()>0 )
			{
				m_nLightInDequeToAssign=0;
				m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(DESIGNOBJ_LightsSetupInclude_CONST),"","",false,false);
			}
			else
				m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);

			return false;
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_AlarmPanel(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_AlarmPanel(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmPanel_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbOnTimer, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmPanel_OnTimer, new CallBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmPanel_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbCapturedKeyboardBufferChanged, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmPanel_CapturedKeyboardBufferChanged, new ObjectInfoBackData());
	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &OSDScreenHandler::AlarmPanel_Intercepted, new MsgInterceptorCellBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_Intercepted(CallBackData *pData)
{
	if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_AlarmDetect_CONST)
	{
		MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;

		if(pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT &&
           pMsgInterceptorCellBackData->m_pMessage->m_dwID == EVENT_Reporting_Child_Devices_CONST )
		{
			string sResult = pMsgInterceptorCellBackData->m_pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
			if(!sResult.size())
			{
				m_pWizardLogic->m_bAlarmPanelIsOk = true; //sensors discovered
				return false;
			}
			else
			{
				m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmNoPanelDetected_CONST), "", "", false, false);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sResult);
				m_pOrbiter->CMD_Refresh("*");
				return false; //wrong port or device
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AlarmPanel_CONST:
		{
			if(DESIGNOBJ_butAlarmPanelModel_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sAlarmManuf = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				if(sAlarmManuf == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_AlarmPanelModel_CONST:
		{
			if(DESIGNOBJ_butAlarmPanelPort_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sAlarmModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				if(sAlarmModel == "")
					return true;
			}
		}
		break;

		case DESIGNOBJ_SelectPort_CONST:
		{
			if(DESIGNOBJ_butAlarmDetect_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sAlarmPort = m_pOrbiter->m_mapVariable[VARIABLE_Datagrid_Input_CONST];
				if(sAlarmPort == "")
					return true;

				string::size_type pos=0;
				int iPK_Device_ControlledVia = atoi(StringUtils::Tokenize(sAlarmPort,",",pos).c_str());
				string sPort = StringUtils::Tokenize(sAlarmPort,",",pos);

				string sAlarmModel = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_3_CONST];
				string sDeviceData = StringUtils::ltos(DEVICEDATA_COM_Port_on_PC_CONST) + "|" + sPort;

				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_Security_Interface_CONST);
				m_pWizardLogic->m_nPK_Device_AlarmPanel = m_pWizardLogic->AddDevice(atoi(sAlarmModel.c_str()), sDeviceData,
                                                                                    iPK_Device_ControlledVia);
// HACK!!!  This damn DSC panel doesn't seem to be able to report it's sensors automatically.  We're just going to fake it for now
                {
                    m_pWizardLogic->AddDevice(DEVICETEMPLATE_Door_Sensor_CONST,
                                              StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "|" + StringUtils::itos(FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST)
                                              + "|" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|3",
                                              m_pWizardLogic->m_nPK_Device_AlarmPanel);
                    m_pWizardLogic->AddDevice(DEVICETEMPLATE_Motion_Detector_CONST,
                                              StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "|" + StringUtils::itos(FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST)
                                              + "|" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|1",
                                              m_pWizardLogic->m_nPK_Device_AlarmPanel);
                    m_pWizardLogic->AddDevice(DEVICETEMPLATE_Doorbell_button_CONST,
                                              StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) + "|" + StringUtils::itos(FLOORPLANOBJECTTYPE_SECURITY_INTERCOM_CONST)
                                              + "|" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|8",
                                              m_pWizardLogic->m_nPK_Device_AlarmPanel);
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
                    m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, StringUtils::ltos(m_pWizardLogic->m_nPK_Device_AlarmPanel));

                    m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmSensors_CONST), "", "", false, false);
                    m_pOrbiter->CMD_Refresh("*");
                    return true;
                }

				m_pWizardLogic->m_bAlarmPanelCommandReceived = false;
				m_pWizardLogic->m_bAlarmPanelIsOk = false;
				m_pWizardLogic->m_nAlarmDeviceTimeout = 210 * 1000;// 3 min and 1/2 timeout

				g_pPlutoLogger->Write(LV_WARNING, "AddDevice called for alarm device %d", m_pWizardLogic->m_nPK_Device_AlarmPanel);
				m_pWizardLogic->m_bAlarmPanelDetectionStarted = false;
			}
		}
		break;

		case DESIGNOBJ_AlarmDetect_CONST:
		{
			if(!m_pWizardLogic->m_bAlarmPanelDetectionStarted)
			{
				m_pWizardLogic->m_bAlarmPanelDetectionStarted = true;
                m_pOrbiter->StartScreenHandlerTimer(500);

				g_pPlutoLogger->Write(LV_WARNING, "Started detection timer for alarm device %d", m_pWizardLogic->m_nPK_Device_AlarmPanel);

				m_pOrbiter->RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter::ScreenHandlerMsgInterceptor),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Reporting_Child_Devices_CONST);

				g_pPlutoLogger->Write(LV_WARNING, "Registered message interceptor for %d", m_pWizardLogic->m_nPK_Device_AlarmPanel);
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_OnTimer(CallBackData *pData)
{
	if(!m_pWizardLogic->m_nPK_Device_AlarmPanel)
		return true;
	else if(!m_pWizardLogic->m_bAlarmPanelCommandReceived)
	{
		string sResponse;
		DCE::CMD_Report_Child_Devices CMD_Report_Child_Devices_(m_pOrbiter->m_dwPK_Device,
                                                                m_pWizardLogic->m_nPK_Device_AlarmPanel);

		if(!m_pOrbiter->SendCommand(CMD_Report_Child_Devices_, &sResponse) || sResponse != "OK" )
			g_pPlutoLogger->Write(LV_WARNING, "Alarm panel is NOT registered. We'll try again later");
		else
		{
			g_pPlutoLogger->Write(LV_WARNING, "Alarm panel is registered. We'll wait to be notified with Reporting event.");
			m_pWizardLogic->m_bAlarmPanelCommandReceived = true;
		}
	}
	else{
		m_pWizardLogic->m_nAlarmDeviceTimeout -= 500;

		g_pPlutoLogger->Write(LV_WARNING, "Check status for device %d... Milliseconds left: %d",
                              m_pWizardLogic->m_nPK_Device_AlarmPanel, m_pWizardLogic->m_nAlarmDeviceTimeout);

		if(m_pWizardLogic->m_bAlarmPanelIsOk)
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, StringUtils::ltos(m_pWizardLogic->m_nPK_Device_AlarmPanel));

			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmSensors_CONST), "", "", false, false);
			m_pOrbiter->CMD_Refresh("*");
			return false; //it's our guy
		}

		if(m_pWizardLogic->m_nAlarmDeviceTimeout <= 0)
		{
			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmNoPanelDetected_CONST), "", "", false, false);
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "The alarm panel does not respond");
			m_pOrbiter->CMD_Refresh("*");
			return false; //wrong port or device
		}
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AlarmSensors_CONST:
		{
			switch(pCellInfoData->m_nPK_Datagrid)
			{
				case DATAGRID_Devices_Children_Of_CONST:
				{
					string sSensorId = pCellInfoData->m_sValue;
					string sSensorName = m_pWizardLogic->GetDeviceName(sSensorId);

					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sSensorName);
					m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()),
                                             sSensorName, TEXT_USR_ENTRY_CONST);
					m_pOrbiter->CMD_Refresh("");

					long FK_Room = m_pWizardLogic->GetRoomForDevice(sSensorId);
					if(FK_Room)
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, StringUtils::ltos(FK_Room));
					else
						m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
					RefreshDatagrid(DESIGNOBJ_dgEditRooms_CONST);

					long FK_DeviceTemplate = m_pWizardLogic->GetDeviceTemplateForDevice(sSensorId);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, StringUtils::ltos(FK_DeviceTemplate));
					RefreshDatagrid(DESIGNOBJ_dgAlarmSensorTypes_CONST);
				}
				break;

				case DATAGRID_Rooms_CONST:
				{
					string sSensorId = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
					string sPK_Room = pCellInfoData->m_sValue;

					m_pWizardLogic->SetRoomForDevice(sSensorId, sPK_Room);
					RefreshDatagrid(DESIGNOBJ_dgAlarmSensors_CONST);
				}
				break;

				case DATAGRID_Sensor_Type_CONST:
				{
					string sSensorId = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
					string sPK_DeviceTemplate = pCellInfoData->m_sValue;

					m_pWizardLogic->ChangeDeviceTemplateForDevice(sSensorId, sPK_DeviceTemplate);
					RefreshDatagrid(DESIGNOBJ_dgAlarmSensors_CONST);
				}
				break;
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_AlarmSensors_CONST:
		{
			string sSensorId = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
			string sName = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];

			if(sName != "")
			{
				m_pWizardLogic->SetDeviceName(sSensorId, sName);
				RefreshDatagrid(DESIGNOBJ_dgAlarmSensors_CONST);
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
// VOIP Provider
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_VOIP_Provider(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

	ScreenHandlerBase::SCREEN_VOIP_Provider(PK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::VOIP_Provider_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::VOIP_Provider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	// Do the start media for the final screen here, rather than in the onload, since the user often
	// comes back to the screen and we don't want to keep reloading each time
	if(DESIGNOBJ_butFinal_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
	{
		DCE::CMD_Play_Media CMD_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->TranslateVirtualDevice(DEVICETEMPLATE_VirtDev_Local_Media_Player_CONST),
                                           "/home/monster/final.mpg",0,0,"");
		m_pOrbiter->SendCommand(CMD_Play_Media);
	}

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
                                                                      m_pOrbiter->m_dwPK_Device_TelecomPlugIn, sVOIPProvider, sPhoneNumber, sPassword, sUsername);
				m_pOrbiter->SendCommand(CMD_PL_Add_VOIP_Account_);
			}
		}
		break;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizardCreate( CallBackData *pData )
{
    g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::RoomsWizardCreate()" );

    PositionCallBackData * pPositionCallBackData = dynamic_cast<PositionCallBackData *>( pData );
    if (pPositionCallBackData == NULL)
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "OSDScreenHandler::RoomsWizardCreate(), NULL Data");
        return false;
    }
    PlutoRectangle plutoRect = pPositionCallBackData->m_rectPosition;
    g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::RoomsWizardCreate(), x=%d, y=%d, w=%d, h=%d",
                           plutoRect.X, plutoRect.Y, plutoRect.Width, plutoRect.Height );
#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
    {
        CallBackData *pCallBackData = new RoomWizardCallBackData(m_pWizardLogic, plutoRect);
        Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogCreate, E_Dialog_RoomWizard, pCallBackData);
        TaskManager::Instance().AddTaskAndWait(pTask);
    }
    {
        CallBackData *pCallBackData = new RoomWizardCallBackData(m_pWizardLogic, plutoRect);
        Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, E_Dialog_RoomWizard, pCallBackData);
        TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
    wxDialog_RoomWizard *pwxDialog = Safe_CreateUnique<wxDialog_RoomWizard>(m_pWizardLogic);
    Safe_Show(pwxDialog);
    wxDialog_RoomWizard::Data_Refresh data_refresh = { wxRect(plutoRect.X, plutoRect.Y, plutoRect.Width, plutoRect.Height) };
    Safe_Gui_Refresh(pwxDialog, &data_refresh);
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

    g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::RoomsWizardCreate() END" );
    return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizardDelete( CallBackData *pData )
{
    g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::RoomsWizardDelete()" );
#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
    {
        CallBackData *pCallBackData = new RoomWizardCallBackData(m_pWizardLogic, PlutoRectangle());
        Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogSave, E_Dialog_RoomWizard, pCallBackData);
        TaskManager::Instance().AddTask(pTask);
    }
    {
        CallBackData *pCallBackData = new RoomWizardCallBackData(m_pWizardLogic, PlutoRectangle());
        Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogDelete, E_Dialog_RoomWizard, pCallBackData);
        TaskManager::Instance().AddTaskAndWait(pTask);
    }
    {
        // when AppDesktop restores main window
        OrbiterLinux *pOrbiterLinux = dynamic_cast<OrbiterLinux *>(m_pOrbiter);
        if(NULL != pOrbiterLinux)
            WMController::Instance().SetVisible("dialog.dialog", false);
    }
#else // (USE_TASK_MANAGER)
    wxDialog_RoomWizard *pwxDialog = ptr_wxDialogByType<wxDialog_RoomWizard>();
    Safe_Gui_DataSave(pwxDialog, m_pWizardLogic);
    Safe_Close(pwxDialog);
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)
    return false;
}

//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::RoomsWizardRefresh( CallBackData *pData )
{
    PositionCallBackData * pPositionCallBackData = dynamic_cast<PositionCallBackData *>( pData );
    if (pPositionCallBackData == NULL)
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "OSDScreenHandler::RoomsWizardRefresh(), NULL Data");
        return false;
    }
    PlutoRectangle plutoRect = pPositionCallBackData->m_rectPosition;
    g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::RoomsWizardRefresh(), x=%d, y=%d, w=%d, h=%d",
                           plutoRect.X, plutoRect.Y, plutoRect.Width, plutoRect.Height );

#if (USE_WX_LIB)
#if (USE_TASK_MANAGER)
    {
        CallBackData *pCallBackData = new RoomWizardCallBackData(m_pWizardLogic, plutoRect);
        Task *pTask = TaskManager::Instance().CreateTask(cbOnDialogRefresh, E_Dialog_RoomWizard, pCallBackData);
        TaskManager::Instance().AddTask(pTask);
    }
#else // (USE_TASK_MANAGER)
    wxDialog_RoomWizard *pwxDialog = ptr_wxDialogByType<wxDialog_RoomWizard>();
    wxDialog_RoomWizard::Data_Refresh data_refresh = { wxRect(plutoRect.X, plutoRect.Y, plutoRect.Width, plutoRect.Height) };
    Safe_Gui_Refresh(pwxDialog, &data_refresh);
#endif // (USE_TASK_MANAGER)
#endif // (USE_WX_LIB)

    g_pPlutoLogger->Write( LV_CRITICAL, "OSDScreenHandler::RoomsWizardRefresh(), END");
    return false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuSpeedControl(long PK_Screen)
{
	g_pPlutoLogger->Write( LV_WARNING, "OSDScreenHandler::SCREEN_DVDRemote()" );
	ScreenHandler::SCREEN_mnuSpeedControl(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::SpeedControlCustomRender, new RenderScreenCallBackData() );
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::SpeedControlCustomRender(CallBackData *pData)
{
	g_pPlutoLogger->Write(LV_WARNING, "OSDScreenHandler::SpeedControlCustomRender()");

	//we don't need this right now
	RenderScreenCallBackData *pRenderData = dynamic_cast<RenderScreenCallBackData *>(pData);
	if(NULL == pRenderData || NULL == pRenderData->m_pObj)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "OSDScreenHandler::SpeedControlCustomRender() : "
			"pData is not a RenderScreenCallBackData or the designobj within is NULL: renderdata %p", pRenderData);
		return false;
	}

	DesignObj_Orbiter *pObj = pRenderData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	//get the background image
	static PlutoGraphic *pLocalBackgroundGraphic = NULL;
	if(NULL == pLocalBackgroundGraphic)
	{
		pLocalBackgroundGraphic = m_pOrbiter->GetBackground(rectTotal);
	}

	pBackgroundGraphic = pLocalBackgroundGraphic;
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	m_pOrbiter->RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );

#ifdef ENABLE_MOUSE_BEHAVIOR

	if(NULL != m_pOrbiter && NULL != m_pOrbiter->m_pMouseBehavior)
	{
		const MouseHandler *pMouseHandler = m_pOrbiter->m_pMouseBehavior->GetMouseHandler();
		const SpeedMouseHandler *pcSpeedMouseHandler = dynamic_cast<const SpeedMouseHandler *>(pMouseHandler);
		if(NULL == pcSpeedMouseHandler)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "OSDScreenHandler::SpeedControlCustomRender() : "
				"Unable to get SpeedMouseHandler");
			return false;
		}

		SpeedMouseHandler *pSpeedMouseHandler = const_cast<SpeedMouseHandler *>(pcSpeedMouseHandler);
		pSpeedMouseHandler->DrawInfo();
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL, "OSDScreenHandler::SpeedControlCustomRender() : "
			"MouseBehavior not activated in Orbiter!");
#endif

	VectorDesignObjText::iterator iText;
	for( iText=pObj->m_vectDesignObjText.begin(  ); iText != pObj->m_vectDesignObjText.end(  ); ++iText )
	{
		DesignObjText *pText = *iText;
		if(  pText->m_bPreRender  )
			continue;
		PROFILE_START( ctText );
		TextStyle *pTextStyle = pText->m_mapTextStyle_Find( 0 );
		string TextToDisplay = m_pOrbiter->SubstituteVariables(m_pOrbiter->SubstituteVariables(pText->m_sText, pText->m_pObject, 0, 0), pText->m_pObject, 0, 0);
		m_pOrbiter->RenderText( TextToDisplay, pText, pTextStyle, point );
		PROFILE_STOP( ctText,  "Text ( obj below )" );
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------=======

bool OSDScreenHandler::AV_Devices_CapturedKeyboardBufferChanged(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = NULL; 
	string sEditVal, sSelectedId;

	//PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
	sEditVal = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
	int nSelectedId = 0;
	pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		case DESIGNOBJ_TVWhatDelays_CONST:
		DesignObj_Orbiter *pObj;
		DesignObj_DataGrid *pDatagridObj;
		DataGridCell *pCell;

		pObj = m_pOrbiter->FindObject(DESIGNOBJ_dgTVWhatDelays_CONST);
		if( NULL != pObj )
		{
			pDatagridObj = dynamic_cast<DesignObj_DataGrid *>(pObj);
			string sValues[3];
			/*if(NULL != pCellInfoData)
				sSelectedId = pCellInfoData->m_sValue;*/
			//PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
			sSelectedId = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST];
			nSelectedId = atoi( sSelectedId.c_str() );

			if(NULL != pDatagridObj)
			{
				for(int i=0;i<3;i++)
				{
					if( !pDatagridObj->m_pDataGridTable )
						return false;
					pCell = pDatagridObj->m_pDataGridTable->GetData(1, i);
					if( NULL != pCell )
					{
						sValues[i] = pCell->GetText();
					}
				}
			}
			if( sEditVal.empty() )
				sValues[nSelectedId] = "0";
			else
				 sValues[nSelectedId] = sEditVal;
			m_pWizardLogic->UpdateAVTemplateDelays( sValues[0], sValues[1], sValues[2]);
			m_pOrbiter->CMD_Refresh("*");
		}
		break;

		case DESIGNOBJ_TVHowToTune_CONST:
			m_pWizardLogic->SetAVTemplateNumericEntry( sEditVal );
		break;
	}

	return false;
}

void OSDScreenHandler::SCREEN_TVConfirmOnOffDiscret(long nPK_Screen)
{	
	//show on screen
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_3_CONST, m_pWizardLogic->GetAVTemplateName() ); 
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_4_CONST, "01" ); 
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_5_CONST, m_pWizardLogic->GetManufactureName() ); 
	
	//used for populate datagrid 
	// ManufactureId,TemplateId,DeviceCategory,[Command]
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_2_CONST, 
						StringUtils::ltos(m_pWizardLogic->GetManufacturerId()) + "," + 
						StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()) + "," + 
						StringUtils::ltos(m_pWizardLogic->GetDeviceCategory()) );
	ScreenHandlerBase::SCREEN_TVConfirmOnOffDiscret(nPK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AVIRCodes_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AVIRCodes_DatagridSelected, 
		new DatagridCellBackData());
}

void OSDScreenHandler::SCREEN_TVConfirmOnOffTogle(long nPK_Screen)
{	
	//show on screen
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_3_CONST, m_pWizardLogic->GetAVTemplateName() ); 
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_4_CONST, "01" ); 
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_5_CONST, m_pWizardLogic->GetManufactureName() ); 
	
	//used for populate datagrid
	m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_2_CONST, 
		StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()) ); 

	ScreenHandlerBase::SCREEN_TVConfirmOnOffTogle(nPK_Screen);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AVIRCodes_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AVIRCodes_DatagridSelected, 
		new DatagridCellBackData());
}


void OSDScreenHandler::SCREEN_TVOnOffCodes(long nPK_Screen)
{
	ScreenHandlerBase::SCREEN_TVOnOffCodes(nPK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::AVIRCodes_ObjectSelected, 
		new ObjectInfoBackData());
}

bool OSDScreenHandler::AVIRCodes_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *> (pData);
	if( !pCellInfoData )
		return false;
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( DESIGNOBJ_dgTVConfirmOnOffCodes_CONST );
	if( !pObj )
		return false;
	DataGridTable * pDataGrid = pObj->m_pDataGridTable;
	if( !pDataGrid )
		return false;
	DataGridCell *pCell = NULL;

	string aux,sSelectId,sSelectPos;
	string::size_type pos = 0;
	
	aux = pCellInfoData->m_sValue;
	sSelectId = StringUtils::Tokenize(aux, ",", pos);
	sSelectPos = StringUtils::Tokenize(aux, ",", pos);
	int test = m_pWizardLogic->GetParentDevice();

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVConfirmOnOffDiscrete_CONST:
		pCell = pDataGrid->GetData(2,atoi(sSelectPos.c_str()));
		if( pCell )
		{
			aux = pCell->GetText();
			CMD_Send_Code cmd(  m_pOrbiter->m_dwPK_Device, 52, pCell->GetText() );
			CMD_Send_Code cmd2(  m_pOrbiter->m_dwPK_Device, 54, pCell->GetText() );

			m_pOrbiter->SendCommand(cmd);
			m_pOrbiter->SendCommand(cmd2);

			/*CMD_Send_Code cmd(  m_pOrbiter->m_dwPK_Device, m_pWizardLogic->GetParentDevice(),
				pCell->GetText() );
			m_pOrbiter->SendCommand(cmd);*/
		}
		break;

		case DESIGNOBJ_TVConfirmOnOffToggle_CONST:
			//m_pWizardLogic->SetIRGroupPower(nIdSelected);
		break;

		case DESIGNOBJ_TVOnOffCodes_CONST:
		break;
	}

	return false;
}

bool OSDScreenHandler::AVIRCodes_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *> (pData);
	if( !pData )
		return false;
	
	//localhost 0 '.$deviceToReceive.' 1 191 9 "'.$code.'"';
	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVConfirmOnOffDiscrete_CONST:
		switch( pObjectInfoData->m_PK_DesignObj_SelectedObject  )
		{
			case DESIGNOBJ_butTVMultipleInputs_CONST:
			m_pWizardLogic->SetAVTemplateTogglePower(false);
			m_pWizardLogic->UpdateAVTemplateToggle();
			return false;

			case DESIGNOBJ_butIRCodeWorks_CONST:
			m_pWizardLogic->SetIRGroupPower( 
				atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST].c_str()) );
			return false;

			case DESIGNOBJ_butIRCodeDoesntwork_CONST:
			return false;
			//m_pOrbiter->SendMessage(
			// save to database
		}
		break;

		case DESIGNOBJ_TVConfirmOnOffToggle_CONST:
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVMultipleInputs_CONST )
			{
				m_pWizardLogic->SetAVTemplateTogglePower(true);
				m_pWizardLogic->UpdateAVTemplateToggle();
				// save to database
			}
		break;

		case DESIGNOBJ_TVOnOffCodes_CONST:
		m_pWizardLogic->SetIRGroupPower(0);
			switch( pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				case DESIGNOBJ_butDiscreteOnOff_CONST:
					m_pWizardLogic->SetAVTemplateTogglePower(false);
				return false;

				case DESIGNOBJ_butToggleOnOff_CONST:
					m_pWizardLogic->SetAVTemplateTogglePower(true);
				return false;
			}
		m_pWizardLogic->UpdateAVTemplateToggle();
		break;
	}

	return false;
}


//AV Wizard Inputs
void OSDScreenHandler::SCREEN_TVMultipleInputs(long nPK_Screen)
{
	ScreenHandlerBase::SCREEN_TVMultipleInputs(nPK_Screen);

	RegisterCallBack(cbObjectSelected, 
		(ScreenHandlerCallBack) &OSDScreenHandler::TVMultipleInputs_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, 
		(ScreenHandlerCallBack) &OSDScreenHandler::TVMultipleInputs_DatagridSelected, 
		new DatagridCellBackData());
}

bool OSDScreenHandler::TVMultipleInputs_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	string aux;

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		//Inputs single/multiple?
		case DESIGNOBJ_TVMultipleInputs_CONST:
			m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_1_CONST, "" );
			m_pOrbiter->CMD_Set_Variable( VARIABLE_Misc_Data_2_CONST, "" );
		return false;

		//Input single
		case DESIGNOBJ_TVSelectMediaType_CONST:
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVDSPMode_CONST )
			{
				if( m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].empty() || 
					(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST] == "0") )
					return true;
				else
				{
					//	m_pWizardLogic->AddAVMediaType();
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, 
					StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()));
					return false;
				}
			}
		return false;

		//Input multiple screen 1
		case DESIGNOBJ_TVInputsNotListed_CONST:
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butConnectorType_CONST )
			{
				aux = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
				aux = StringUtils::Replace( aux, "|", "," );

				//reset the selection for the first datagrid
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "" );
				// parameter for select first datagrid
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, aux );
				//test if selection
				if( m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST].empty() )
					return true;
				else
					return false;
			}
		return false;

		// Input multiple screen 2
		case DESIGNOBJ_TVConnectorType_CONST:
			if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSQLInputCodes_CONST )
			{
				m_pWizardLogic->AddAVDeviceInput();
				switch( m_pWizardLogic->GetAVInputType() )
				{
					case 0:
						m_pOrbiter->GotoDesignObj( StringUtils::ltos(DESIGNOBJ_TVInputsCodes_CONST) );
					break;

					case 1:
						m_pOrbiter->GotoDesignObj( StringUtils::ltos(DESIGNOBJ_TVConfirmInputsToggle_CONST) );
					break;

					case 2:
						m_pOrbiter->GotoDesignObj( StringUtils::ltos(DESIGNOBJ_TVConfirmInputsDiscrete_CONST) );
					break;
				}
			}

		case DESIGNOBJ_TVConfirmInputsDiscrete_CONST:
		break;

		case DESIGNOBJ_TVConfirmInputsToggle_CONST:
		aux = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST];  // debug only
		if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVInputsOrder_CONST )
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, 
				StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()) );
		break;

		case DESIGNOBJ_TVInputsCodes_CONST:
		break;

		case DESIGNOBJ_TVInputsOrder_CONST:
		switch( pObjectInfoData->m_PK_DesignObj_SelectedObject )
		{
			case DESIGNOBJ_butIRGroups_CONST:
			return false;

			//up
			case DESIGNOBJ_butOrderMoveUp_CONST:
			if( GridSwapPos(DESIGNOBJ_dgInputsOrder_CONST,-1,VARIABLE_Misc_Data_1_CONST) )
			{
				m_pWizardLogic->ChangeInputOrder();
				RefreshDatagrid(DESIGNOBJ_dgInputsOrder_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,m_pWizardLogic->m_SelectId);
			}
			return false;

			//down
			case DESIGNOBJ_butOrderMoveDown_CONST:
			if( GridSwapPos(DESIGNOBJ_dgInputsOrder_CONST,1,VARIABLE_Misc_Data_1_CONST) )
			{
				m_pWizardLogic->ChangeInputOrder();
				RefreshDatagrid(DESIGNOBJ_dgInputsOrder_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,m_pWizardLogic->m_SelectId);
			}
			return false;
		}
		return false;
	}

	return false;
}

bool OSDScreenHandler::TVMultipleInputs_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *> (pData);
	string sSelectId;
	string firstId, secondId, aux;
	int nIdSelected = 0;
	if(NULL != pCellInfoData)
		sSelectId = pCellInfoData->m_sValue;
	nIdSelected = atoi( sSelectId.c_str() );

	switch( GetCurrentScreen_PK_DesignObj() )
	{
		// Single Input
		case DESIGNOBJ_TVSelectMediaType_CONST:
			m_pWizardLogic->SetPKMediaType(nIdSelected);
		return false;

		//Multiple input  screen 1
		case DESIGNOBJ_TVInputsNotListed_CONST:
		return false;

		//Multiple input  screen 2
		case DESIGNOBJ_TVConnectorType_CONST:
		switch( pCellInfoData->m_nPK_Datagrid )
		{
			case DATAGRID_Select_Available_Inputs_CONST:
			m_pWizardLogic->m_AVInputsId[0] = sSelectId;

			//first time select 
			if( m_pWizardLogic->m_mapAVInputs[nIdSelected].empty() )
			{
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "0" );
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "0" );
				m_pWizardLogic->m_AVInputsId[1] = "0";
				m_pWizardLogic->m_AVInputsId[2] = "0";

				m_pWizardLogic->m_mapAVInputs[nIdSelected] = "0,0";
			}
			else
			{		
				string::size_type pos=0;
				aux = m_pWizardLogic->m_mapAVInputs[nIdSelected];
				firstId = StringUtils::Tokenize( aux, ",",pos );
				secondId = StringUtils::Tokenize( aux, ",",pos );
				
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, firstId );
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, secondId );
				m_pWizardLogic->m_AVInputsId[1] = firstId;
				m_pWizardLogic->m_AVInputsId[2] = secondId;
			}

			RefreshDatagrid( DESIGNOBJ_dgMediaType_CONST );
			RefreshDatagrid( DESIGNOBJ_dgConnectorType_CONST );
			return false;

			case DATAGRID_Media_Type_CONST:
			m_pWizardLogic->m_AVInputsId[1] = sSelectId;

			aux = m_pWizardLogic->m_AVInputsId[1] + "," + m_pWizardLogic->m_AVInputsId[2];
			m_pWizardLogic->m_mapAVInputs[atoi(m_pWizardLogic->m_AVInputsId[0].c_str())] = aux;
			return false;

			case DATAGRID_Media_Connector_Type_CONST:
			m_pWizardLogic->m_AVInputsId[2] = sSelectId;
			
			aux = m_pWizardLogic->m_AVInputsId[1] + "," + m_pWizardLogic->m_AVInputsId[2];
			m_pWizardLogic->m_mapAVInputs[atoi(m_pWizardLogic->m_AVInputsId[0].c_str())] = aux;
			return false;
		}
	}

	return false;
}


//AV Wizard DSP Mode
void OSDScreenHandler::SCREEN_TVDSPMode(long nPK_Screen)
{
	ScreenHandlerBase::SCREEN_TVMultipleInputs(nPK_Screen);

	RegisterCallBack(cbObjectSelected, 
		(ScreenHandlerCallBack) &OSDScreenHandler::TVDSPMode_ObjectSelected, 
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, 
		(ScreenHandlerCallBack) &OSDScreenHandler::TVDSPMode_DatagridSelected, 
		new DatagridCellBackData());
}

//
bool OSDScreenHandler::GridSwapPos(int gridId,int nDiff,int nSelectId)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(StringUtils::ltos(gridId));
	DataGridTable * pDataGrid = NULL;
	DataGridCell *pCell = NULL;

	string firstId,secondId;
	string aux;
	string::size_type pos = 0;
	int nFirstPos = 0,nSecondPos = 0;

	m_pWizardLogic->m_firstId = m_pWizardLogic->m_secondId = "";
	m_pWizardLogic->m_firstPos = m_pWizardLogic->m_secondPos = "";
	if( !pObj )
		return false;
	pDataGrid = pObj->m_pDataGridTable;

	aux = m_pOrbiter->m_mapVariable[nSelectId];			
	firstId = StringUtils::Tokenize(aux, ",", pos);
	nFirstPos = atoi( StringUtils::Tokenize(aux, ",", pos).c_str() );
	nSecondPos = nFirstPos + nDiff;

	if( !pDataGrid )
		return false;
	if( (nSecondPos < 0) || (nSecondPos > pDataGrid->GetRows()) )
		return  false;
	pCell = pDataGrid->GetData(0, nSecondPos );
	if( !pCell )
		return false;
	aux = pCell->GetValue();
	pos = 0;
	secondId = StringUtils::Tokenize(aux, ",", pos);
	
	m_pWizardLogic->m_firstId = firstId;
	m_pWizardLogic->m_secondId = secondId;
	m_pWizardLogic->m_firstPos = StringUtils::ltos(nFirstPos);
	m_pWizardLogic->m_secondPos = StringUtils::ltos(nSecondPos);

	m_pWizardLogic->m_SelectId = firstId + "," + StringUtils::ltos(nSecondPos); 
	return true;
}

bool OSDScreenHandler::TVDSPMode_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	string sSelectIds,id;
	string::size_type pos = 0;
	list<string>::iterator it;
	int nPos = 0;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVDspMode_CONST:
			switch( pObjectInfoData->m_PK_DesignObj_SelectedObject )
			{
				case DESIGNOBJ_butNoDSPModes_CONST:
				if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butNoDSPModes_CONST)
				{
					if( m_pWizardLogic->IsIRCodes() )
						m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_IRGroup_CONST) );
					else
						m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_LearnIRCodes_CONST) );
					return true;
				}

				case DESIGNOBJ_butDiscreteCodesDSPModes_CONST:
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,"");
				m_pWizardLogic->SetAVToggleDSP(false);
				m_pWizardLogic->UpdateAVTemplateToggle();
				return false;

				case DESIGNOBJ_butToggleCodesDSPModes_CONST:
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,"");
				m_pWizardLogic->SetAVToggleDSP(true);
				m_pWizardLogic->UpdateAVTemplateToggle();
				return false;
			}
		return false;

		case DESIGNOBJ_TVDspList_CONST:
		if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVDSPModesList_CONST )
		{
			sSelectIds = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
			sSelectIds = StringUtils::Replace( sSelectIds, "|", "," );
			m_pWizardLogic->m_listDSPModes.clear();

			do
			{
				id = StringUtils::Tokenize(sSelectIds, ",", pos);
				nPos++;
				if( nPos % 2 )
					m_pWizardLogic->m_listDSPModes.push_back( id );
				bool bRes = id.empty();
			}
			while( id.empty() == false );
			m_pWizardLogic->InsertDSPModes();

			if( m_pWizardLogic->GetAVToggleDSP() == false ) 
			{
				if( m_pWizardLogic->IsIRCodes() )
					m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_IRGroup_CONST) );
				else
					m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_LearnIRCodes_CONST) );
				return true;
			}
		}
		return false;

		case DESIGNOBJ_TVDspOrder_CONST:
		switch( pObjectInfoData->m_PK_DesignObj_SelectedObject )
		{
			case DESIGNOBJ_butCodes_CONST:
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,
				StringUtils::ltos(m_pWizardLogic->GetManufacturerId()) + "," + 
				StringUtils::ltos(m_pWizardLogic->GetDeviceCategory()) );

			if( m_pWizardLogic->IsIRCodes() )
					m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_IRGroup_CONST) );
				else
					m_pOrbiter->GotoDesignObj(StringUtils::ltos(DESIGNOBJ_LearnIRCodes_CONST) );
			return true;

			//up
			case DESIGNOBJ_butOrderMoveUp_CONST:
			if( GridSwapPos(DESIGNOBJ_dgDSPModesOrder_CONST,-1,VARIABLE_Misc_Data_1_CONST) )
			{
				m_pWizardLogic->ChangeDSPOrder();
				RefreshDatagrid(DESIGNOBJ_dgDSPModesOrder_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,m_pWizardLogic->m_SelectId);
			}
			return false;

			//down
			case DESIGNOBJ_butOrderMoveDown_CONST:
			if( GridSwapPos(DESIGNOBJ_dgDSPModesOrder_CONST,1,VARIABLE_Misc_Data_1_CONST) )
			{
				m_pWizardLogic->ChangeDSPOrder();
				RefreshDatagrid(DESIGNOBJ_dgDSPModesOrder_CONST);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,m_pWizardLogic->m_SelectId);
			}
			return false;
		}
		return false;

		case DESIGNOBJ_IRGroup_CONST:
		id = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST];
		switch( pObjectInfoData->m_PK_DesignObj_SelectedObject )
		{
			case DESIGNOBJ_butIRCodesSetWorks_CONST:
			m_pWizardLogic->m_listIRGroups.insert(atoi(id.c_str()));
			return false;
			case DESIGNOBJ_butIRCodesSetDoesntwork_CONST:
			m_pWizardLogic->m_listIRGroups.erase( atoi(id.c_str()) ); 
			return false;
			case DESIGNOBJ_butConfirmIRCodes_CONST:
			set<int>::iterator it;
			id = "";
			for(it=m_pWizardLogic->m_listIRGroups.begin();it!=m_pWizardLogic->m_listIRGroups.end();it++)
				id += StringUtils::ltos(*it) + ",";
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,id);
			id[id.size()-1] = ' ';
			return false;
		}
		return false;

		case DESIGNOBJ_LearnIRCodes_CONST:
		if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butTVReceiver_CONST )
		{
		//start learn
		CMD_Learn_IR cmd(m_pOrbiter->m_dwPK_Device, 52,
			0, "1", 0, 192);
		m_pOrbiter->SendCommand(cmd);
		}
		return false;
}

	return false;
}

bool OSDScreenHandler::TVDSPMode_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *> (pData);
	string sSelectId;
	int nIdSelected = 0;
	if(NULL != pCellInfoData)
		sSelectId = pCellInfoData->m_sValue;
	nIdSelected = atoi( sSelectId.c_str() );
	string aux;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_TVDspMode_CONST:
		return false;

		case DESIGNOBJ_TVDspList_CONST:
		return false;

		case DESIGNOBJ_TVDspOrder_CONST:
		return false;

		case DESIGNOBJ_IRGroup_CONST:
		switch( pCellInfoData->m_nPK_Datagrid )
		{
			case DATAGRID_IR_Codes_Sets_CONST:
			aux = StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()) + "," + sSelectId;
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST,
				StringUtils::ltos(m_pWizardLogic->GetAVTemplateId()) + "," + sSelectId );
			RefreshDatagrid(DESIGNOBJ_dgIRCodes_CONST);
			return false;
		}
		return false;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuVolume(long PK_Screen)
{
	g_pPlutoLogger->Write( LV_WARNING, "ScreenHandler::SCREEN_mnuAmbiance" );
	ScreenHandlerBase::SCREEN_mnuVolume(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::VolumeControlCustomRender, new RenderScreenCallBackData() );
}

bool OSDScreenHandler::VolumeControlCustomRender(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	DesignObj_Orbiter *pObj = pRenderScreenCallBackData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		//hook the object to have the background image merged with the desktop image
		//will do this only once
		static PlutoGraphic *pBackgroundBackgroundX = NULL;
		static PlutoGraphic *pBackgroundBackgroundY = NULL;

		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			if(NULL == pBackgroundBackgroundX)
				pBackgroundBackgroundX = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundX;
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			if(NULL == pBackgroundBackgroundY)
				pBackgroundBackgroundY = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundY;
		} 
		*/
	} 
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	m_pOrbiter->RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			VolumeMouseHandler *pVolumeMouseHandler = dynamic_cast<VolumeMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler);
			pVolumeMouseHandler->CustomRender();
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical);
			pLightMouseHandler->CustomRender();
		}
		*/
	}
#endif
	return false;
}




//-----------------------------------------------------------------------------------------------------
/*virtual*/ void OSDScreenHandler::SCREEN_mnuLights(long PK_Screen)
{
	g_pPlutoLogger->Write( LV_WARNING, "ScreenHandler::SCREEN_mnuAmbiance" );
	ScreenHandlerBase::SCREEN_mnuLights(PK_Screen);
	
	RegisterCallBack( cbOnCustomRender, (ScreenHandlerCallBack)&OSDScreenHandler::LightControlCustomRender, new RenderScreenCallBackData() );
}

bool OSDScreenHandler::LightControlCustomRender(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	DesignObj_Orbiter *pObj = pRenderScreenCallBackData->m_pObj;
	PlutoRectangle rectTotal = pObj->m_rPosition;
	PlutoPoint point;

	PlutoGraphic *pBackgroundGraphic = NULL;

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		//hook the object to have the background image merged with the desktop image
		//will do this only once
		static PlutoGraphic *pBackgroundBackgroundX = NULL;
		static PlutoGraphic *pBackgroundBackgroundY = NULL;

		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			if(NULL == pBackgroundBackgroundX)
				pBackgroundBackgroundX = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundX;
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			if(NULL == pBackgroundBackgroundY)
				pBackgroundBackgroundY = m_pOrbiter->GetBackground(rectTotal);

			pBackgroundGraphic = pBackgroundBackgroundY;
		} 
		*/
	} 
#endif

	if(NULL != pBackgroundGraphic)
		m_pOrbiter->RenderGraphic( pBackgroundGraphic,  rectTotal, pObj->m_bDisableAspectLock, point );

	m_pOrbiter->RenderGraphic( pObj,  rectTotal, pObj->m_bDisableAspectLock, point );

#ifdef ENABLE_MOUSE_BEHAVIOR
	if( m_pOrbiter->m_pMouseBehavior )
	{
		if( m_pOrbiter->m_pMouseBehavior->m_pMouseHandler )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler);
			pLightMouseHandler->CustomRender();
		}
		/*
		else if( m_pOrbiter->m_pMouseBehavior->m_cLocked_Axis_Current==AXIS_LOCK_Y && m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical )
		{
			LightMouseHandler *pLightMouseHandler = dynamic_cast<LightMouseHandler *> (m_pOrbiter->m_pMouseBehavior->m_pMouseHandler_Vertical);
			pLightMouseHandler->CustomRender();
		}
		*/
	}
#endif
	return false;
}

void OSDScreenHandler::SCREEN_mnuPlaybackControl(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_mnuPlaybackControl(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::PlayBackControlSelected, new ObjectInfoBackData());
}

bool OSDScreenHandler::PlayBackControlSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_hiddenShowSpeedControl_CONST )
		m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Speed);
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject==DESIGNOBJ_hiddenShowMediaTracks_CONST )
		m_pOrbiter->CMD_Goto_Screen("", m_pOrbiter->m_iPK_Screen_OSD_Track);
	return false;
}