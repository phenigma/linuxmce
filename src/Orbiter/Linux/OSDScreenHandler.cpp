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
#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "../WizardLogic.h"

//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
	ScreenHandler(pOrbiter, p_MapDesignObj)
{
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
bool OSDScreenHandler::HandleAddUser()
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
				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(DESIGNOBJ_PostalCode_CONST),TEXT_STATUS_CONST );
				if( pText )
				{
					if( sCityRegion.size()==0 )
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Enter_your_postal_code_CONST];
					else
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_postal_code_CONST];
				}
			}
		}
		break;

		case DESIGNOBJ_PostalCode_CONST:
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butWizRooms_CONST ) 
			{
				PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_VariableMutex);
				string sPostalCode = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];
				if( sPostalCode.size()==0 )
				{
					string sCityRegion = m_pWizardLogic->GetCityRegion();
					if( sCityRegion.size() )
						return false; // Nothing to do, the user is just confirming
				}
				bool bPostalCodeOk = m_pWizardLogic->SetPostalCode(sPostalCode);
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,"");

				string sCityRegion = m_pWizardLogic->GetCityRegion();
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sCityRegion);

				DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(StringUtils::ltos(DESIGNOBJ_PostalCode_CONST)),TEXT_STATUS_CONST );
				if( pText )
				{
					if( bPostalCodeOk )
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_postal_code_CONST];
					else
						pText->m_sText = m_pOrbiter->m_mapTextString[TEXT_Bad_Zip_Code_CONST];
				}
				m_pOrbiter->CMD_Refresh("");
				return true;
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

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_provider_ObjectSelected, new ObjectInfoBackData());
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
					"/usr/pluto/bin/SetupMythHack.sh","",StringUtils::itos(PK_PostalCode),"","",false,false,false);
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

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &OSDScreenHandler::TV_Manufacturer_GridSelected, new DatagridCellBackData());
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
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::TV_Manufacturer_ObjectSelected(CallBackData *pData)
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
		m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()),
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

					int PK_Device_From = 0;
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
			if(DESIGNOBJ_butLightsWizard_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
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
	RegisterCallBack(cbOnTimer, (ScreenHandlerCallBack) &OSDScreenHandler::LightsSetup_Timer, new CallBackData());
}
//-----------------------------------------------------------------------------------------------------

// TODO: This is Monster specific
bool OSDScreenHandler::LightsSetup_OnScreen(CallBackData *pData)
{
	RenderScreenCallBackData *pRenderScreenCallBackData = (RenderScreenCallBackData *)pData;
	if( pRenderScreenCallBackData->m_pObj->m_iBaseObjectID==DESIGNOBJ_LightSetupRooms_CONST )
	{
		if( m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
		{
			DesignObjText *pText = m_pOrbiter->FindText( m_pOrbiter->FindObject(StringUtils::itos(DESIGNOBJ_LightSetupRooms_CONST) + ".0." + StringUtils::itos(DESIGNOBJ_objLightStatus_CONST)),TEXT_STATUS_CONST );
			if( pText )
				pText->m_sText = StringUtils::itos(m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first) + 
					" " + DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first);

			m_pOrbiter->CallMaintenanceInMiliseconds(0,&Orbiter::ServiceScreenHandler,NULL,pe_ALL);
		}
		else
		{
			m_pOrbiter->CMD_Goto_Screen("",SCREEN_AlarmPanel_CONST);
			m_pOrbiter->CMD_Refresh("");
		}
	}
	return false;
}

bool OSDScreenHandler::LightsSetup_Timer(CallBackData *pData)
{
	static bool bLastTimeOn=true;
	if( m_nLightInDequeToAssign < (int) m_pWizardLogic->m_dequeNumLights.size() )
	{
		string sID = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].second;
		int PK_Device = m_pWizardLogic->m_dequeNumLights[m_nLightInDequeToAssign].first;
		string sDescription = DatabaseUtils::GetDescriptionForDevice(m_pWizardLogic,PK_Device);

		string sText = string("Turning ") + (bLastTimeOn ? "OFF " : "ON ") + StringUtils::itos(PK_Device) + " " + sDescription + "...";
		DesignObj_Orbiter *pObjStatus = m_pOrbiter->FindObject(StringUtils::itos(DESIGNOBJ_LightSetupRooms_CONST) + ".0.0." + StringUtils::itos(DESIGNOBJ_objLightStatus_CONST));
		DesignObjText *pText = m_pOrbiter->FindText( pObjStatus,TEXT_STATUS_CONST );
		if( pText )
			pText->m_sText = sText;

		string sResponse;
		if( !bLastTimeOn )
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave, // Monster specific
				sID,COMMAND_Generic_On_CONST,"");

			m_pOrbiter->SendCommand(CMD_Send_Command_To_Child,&sResponse);
		}
		else
		{
			DCE::CMD_Send_Command_To_Child CMD_Send_Command_To_Child(m_pOrbiter->m_dwPK_Device,m_pWizardLogic->m_nPK_Device_ZWave, // Monster specific
				sID,COMMAND_Generic_Off_CONST,"");
			m_pOrbiter->SendCommand(CMD_Send_Command_To_Child,&sResponse);
		}
		bLastTimeOn=!bLastTimeOn;

		if( pText )
			pText->m_sText = sText + "(" + sResponse  + ")";

		PLUTO_SAFETY_LOCK( nd, m_pOrbiter->m_NeedRedrawVarMutex );
		if( pObjStatus )
			m_pOrbiter->m_vectObjs_NeedRedraw.push_back(pObjStatus);
	    NeedToRender render2( m_pOrbiter, "OSDScreenHandler::LightsSetup_Timer2" );  // Redraw anything that was changed by this command
		m_pOrbiter->CallMaintenanceInMiliseconds(4000,&Orbiter::ServiceScreenHandler,NULL,pe_ALL);
	}
	return false;
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

				m_pWizardLogic->m_nPK_Device_AlarmPanel = m_pWizardLogic->AddDevice(atoi(sAlarmModel.c_str()), sDeviceData,
					iPK_Device_ControlledVia);

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
				case DATAGRID_Alarm_Sensors_CONST:
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
