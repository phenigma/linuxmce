#include "OSDScreenHandler.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "Gen_Devices/AllCommandsRequests.h"

#include "../WizardLogic.h"

//-----------------------------------------------------------------------------------------------------
OSDScreenHandler::OSDScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) :
	ScreenHandler(pOrbiter, p_MapDesignObj)
{
	m_pWizardLogic = new WizardLogic(pOrbiter);

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
void OSDScreenHandler::SCREEN_FileSave(long PK_Screen, string sDefaultUserValue, string sPrivate, string sPublic, string sCaption)
{
	ScreenHandler::SCREEN_FileSave(PK_Screen, sDefaultUserValue, sPrivate, sPublic, sCaption);

	//your stuff
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &OSDScreenHandler::FileSave_ObjectSelected, new PositionCallBackData());	
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	PositionCallBackData *pMyData = dynamic_cast<PositionCallBackData *>(pData);
//	g_pPlutoLogger->Write(LV_WARNING, "FileSave_ObjectSelected, with position %d, %d, %d, %d",
//			pMyData->x, pMyData->y, pMyData->w, pMyData->h);
	
	return false;
}
//-----------------------------------------------------------------------------------------------------


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
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");

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
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_Cable_Boxes_CONST);
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_Satellite_Boxes_CONST);
				m_nPK_Device_TVProvider_External = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
				m_pWizardLogic->AddExternalTuner(m_nPK_Device_TVProvider_External);  // Make this point to our PVR card's input
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
				m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_TVsPlasmasLCDsProjectors_CONST);
				m_pWizardLogic->m_nPK_Device_TV = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
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
					m_pWizardLogic->DeleteDevicesInThisRoomOfType(DEVICECATEGORY_AmpsPreampsReceiversTuners_CONST);
					int PK_Receiver = m_pWizardLogic->AddDevice(atoi(sModel.c_str()));

					int PK_Device_From = 0;
					int PK_Command_Input = atoi(sReceiverInputs.c_str());
					
					// Set the input
					m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
						PK_Receiver,1 /* audio */,PK_Command_Input);

					// Video also goes to the receiver, and the receiver's video goes to the tv
					if( m_pWizardLogic->m_bUsingReceiverForVideo )
					{
						m_pWizardLogic->SetAvPath(m_pOrbiter->m_pData->m_dwPK_Device_ControlledVia,
							PK_Receiver,2 /* video */,PK_Command_Input);
						m_pWizardLogic->SetAvPath(PK_Receiver,
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
				m_pWizardLogic->AddDevice(atoi(sModel.c_str()));
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void OSDScreenHandler::SCREEN_LightsSetup(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_LightsSetup(PK_Screen);
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

				m_pWizardLogic->m_nPK_AlarmPanelDevice = m_pWizardLogic->AddDevice(atoi(sAlarmModel.c_str()));
				//this will also spawn the device

				//for testing
				m_pWizardLogic->m_nPK_AlarmPanelDevice = 5237;
			}
		}
		break;

		case DESIGNOBJ_AlarmPanelPort_CONST:
		{
			if(DESIGNOBJ_butAlarmDetect_CONST == pObjectInfoData->m_PK_DesignObj_SelectedObject)
			{
				string sAlarmPort = m_pOrbiter->m_mapVariable[VARIABLE_Datagrid_Input_CONST];
				if(sAlarmPort == "")
					return true;

				//write a device data?
				m_pWizardLogic->m_bAlarmPanelDetectionStarted = false;
				m_pWizardLogic->m_nAlarmDeviceTimeout = 15000;//millisec
			}
		}
		break;

		case DESIGNOBJ_AlarmDetect_CONST:
		{
			if(!m_pWizardLogic->m_bAlarmPanelDetectionStarted)
			{
				m_pWizardLogic->m_bAlarmPanelDetectionStarted = true;
                m_pOrbiter->StartScreenHandlerTimer(500);
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool OSDScreenHandler::AlarmPanel_OnTimer(CallBackData *pData)
{
	if(!m_pWizardLogic->m_nPK_AlarmPanelDevice)
		return true;
	else
	{
		m_pWizardLogic->m_nAlarmDeviceTimeout -= 500;

		g_pPlutoLogger->Write(LV_WARNING, "Check status for device %d... Milliseconds left: %d", 
			m_pWizardLogic->m_nPK_AlarmPanelDevice, m_pWizardLogic->m_nAlarmDeviceTimeout);

		string sDeviceStatus = m_pWizardLogic->GetDeviceStatus(m_pWizardLogic->m_nPK_AlarmPanelDevice);
		if(sDeviceStatus == "READY")
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Input_CONST, "");
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Datagrid_Filter_CONST, StringUtils::ltos(m_pWizardLogic->m_nPK_AlarmPanelDevice));

			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmSensors_CONST), "", "", false, false);
			m_pOrbiter->CMD_Refresh("*");
			return false; //it's our guy
		}

		if(m_pWizardLogic->m_nAlarmDeviceTimeout <= 0)
		{
			m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_AlarmNoPanelDetected_CONST), "", "", false, false);
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
					/*
					string sSensorValue = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];

					vector<string> vectStrings;
					StringUtils::Tokenize(sSensorValue, ",", vectStrings);

					string sSensorName;
					if(vectStrings.size() == 3)
					{
						string sSensorName = vectStrings[0];
					}
					*/

					/*
					string sBubu = pCellInfoData->m_sValue;

					m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, pCellInfoData->m_sValue);
					m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, pCellInfoData->m_sText);
					m_pOrbiter->CMD_Set_Text(StringUtils::ltos(GetCurrentScreen_PK_DesignObj()), 
						pCellInfoData->m_sText, TEXT_USR_ENTRY_CONST);
					*/
				}
				break;

				case DATAGRID_Room_Types_CONST:
				{
				}
				break;

				case DATAGRID_Sensor_Type_CONST:
				{
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
			string sSensorValue = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_4_CONST];
			string sTest = "";

			/*
			int PK_Room = atoi(m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_1_CONST].c_str());
			string sName = m_pOrbiter->m_mapVariable[VARIABLE_Seek_Value_CONST];

			if(sName != "" && PK_Room)
			{
				m_pWizardLogic->ChangeRoomName(PK_Room, sName);
				m_pOrbiter->CMD_Refresh("*");
			}
			*/


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
					m_pOrbiter->m_dwPK_Device_TelecomPlugIn, sVOIPProvider, sPhoneNumber, sUsername, sPassword);
				m_pOrbiter->SendCommand(CMD_PL_Add_VOIP_Account_);
			}
		}
		break;
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
