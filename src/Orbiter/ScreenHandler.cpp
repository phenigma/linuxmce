#include "ScreenHandler.h"
#include "ScreenHistory.h"
#include "CallBackTypes.h"
#include "OrbiterRenderer.h"
#ifdef ENABLE_MOUSE_BEHAVIOR
#include "MouseBehavior.h"
#endif
#include "MediaBrowserMouseHandler.h"
#include "DataGrid.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Message.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_media/Define_AttributeType.h"

using namespace DCE;

//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : 
	ScreenHandlerBase(p_MapDesignObj)
	, m_MapMutex("maps")
	, mediaFileBrowserOptions(pOrbiter)
{
	m_pOrbiter = pOrbiter;
	m_tLastDeviceAdded = 0;
	m_MapMutex.Init(NULL);

	m_nSaveFile_PK_DeviceDrive = 0;
	m_bSaveFile_CreatingFolder = false;
	m_bSaveFile_Advanced_options = true;
}
//-----------------------------------------------------------------------------------------------------
ScreenHandler::~ScreenHandler()
{
	pthread_mutex_destroy(&m_MapMutex.mutex);

	m_pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack,
	CallBackData *pCallBackData)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	m_mapCallBack[aCallBackType] = aScreenHandlerCallBack;
	m_mapCallBackData[aCallBackType] = pCallBackData;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::ResetCallBacks()
{
g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ResetCallBacks");
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
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
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, ScreenHandlerCallBack>::iterator it = m_mapCallBack.find(aCallBackType);
	return it == m_mapCallBack.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
CallBackData *ScreenHandler::m_mapCallBackData_Find(CallBackType aCallBackType)
{
	PLUTO_SAFETY_LOCK(vm, m_MapMutex);
	map<CallBackType, CallBackData *>::iterator it = m_mapCallBackData.find(aCallBackType);
	return it == m_mapCallBackData.end() ? NULL : it->second;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::RefreshDatagrid(long PK_DesignObj_Datagrid)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(StringUtils::ltos(PK_DesignObj_Datagrid));
	if(pObj)
	{
		m_pOrbiter->Renderer()->RenderObjectAsync(pObj);
		m_pOrbiter->InitializeGrid((DesignObj_DataGrid *)pObj);
		m_pOrbiter->Renderer()->RedrawObjects();
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GotoDesignObj(int PK_DesignObj,string sID,bool bStore_Variables,bool bCant_Go_Back)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(PK_DesignObj), sID, "", bStore_Variables, bCant_Go_Back);
}
//-----------------------------------------------------------------------------------------------------
int ScreenHandler::GetCurrentScreen_PK_DesignObj()
{
	return m_pOrbiter->m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
{
	string::size_type pos=0;
	string sTitle = StringUtils::Tokenize(sText,"|",pos);
	string sOption1 = StringUtils::Tokenize(sText,"|",pos);
	string sOption2 = StringUtils::Tokenize(sText,"|",pos);
	string sOption3 = StringUtils::Tokenize(sText,"|",pos);
	string sOption4 = StringUtils::Tokenize(sText,"|",pos);

	pos=0;
	string sMessage1 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage2 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage3 = StringUtils::Tokenize(sCommand_Line,"|",pos);
	string sMessage4 = StringUtils::Tokenize(sCommand_Line,"|",pos);

	DisplayMessageOnOrbiter(PK_Screen,sTitle, sPromptToResetRouter == "1", sTimeout, sCannotGoBack == "1",
		sOption1,sMessage1,sOption2,sMessage2,sOption3,sMessage3,sOption4,sMessage4);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue)
{
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Pnp_ObjectSelected,	new ObjectInfoBackData());
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), StringUtils::itos(iPK_PnpQueue), "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_New_Pnp_Device_One_Possibility(long PK_Screen, int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue)
{
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Pnp_ObjectSelected,	new ObjectInfoBackData());
	string sTitle = m_pOrbiter->m_mapTextString[TEXT_new_pnp_device_one_possibility_CONST];
	string sRoom;
	if( iPK_Room )
	{
		sRoom = m_pOrbiter->m_mapTextString[TEXT_I_think_it_is_in_the_room_CONST];
		StringUtils::Replace(&sRoom,"<%=ROOM%>",ssComments);
	}
	StringUtils::Replace(&sTitle,"<%=ROOM%>",sRoom);
	StringUtils::Replace(&sTitle,"<%=DEVICE%>",sDescription);

	string sMessageYesUseItWrongRoom = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " 1 " + 
		TOSTRING(COMMAND_Choose_Pnp_Device_Template_CONST) + " " + TOSTRING(COMMANDPARAMETER_PK_DHCPDevice_CONST) + " " + StringUtils::itos(iPK_DHCPDevice) +
		" " + TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue);

	string sMessageYesUseIt = sMessageYesUseItWrongRoom +
		" " + TOSTRING(COMMANDPARAMETER_PK_Room_CONST) + " " + StringUtils::itos(iPK_Room);

	string sMessageNoIgnoreIt = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " 1 " + 
		TOSTRING(COMMAND_Ignore_PNP_Device_CONST) + " " + " " + TOSTRING(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue);

	string sMessageNoIgnoreItEveryTime = sMessageNoIgnoreIt + " " + TOSTRING(COMMANDPARAMETER_Always_CONST) + " 1";

	if( iPK_Room )
		DisplayMessageOnOrbiter(PK_Screen,sTitle, false, "0", true,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sMessageYesUseIt,m_pOrbiter->m_mapTextString[TEXT_Yes_but_wrong_room_CONST],sMessageYesUseItWrongRoom,
			m_pOrbiter->m_mapTextString[TEXT_No_ignore_it_CONST],sMessageNoIgnoreIt,m_pOrbiter->m_mapTextString[TEXT_Ignore_everytime_CONST],sMessageNoIgnoreItEveryTime,StringUtils::itos(iPK_PnpQueue));
	else
		DisplayMessageOnOrbiter(PK_Screen,sTitle, false, "0", true,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sMessageYesUseIt,
			m_pOrbiter->m_mapTextString[TEXT_No_ignore_it_CONST],sMessageNoIgnoreIt,m_pOrbiter->m_mapTextString[TEXT_Ignore_everytime_CONST],sMessageNoIgnoreItEveryTime,"","",StringUtils::itos(iPK_PnpQueue));
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Pnp_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST || 
		pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse3_CONST || pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse4_CONST )
	{
		m_tLastDeviceAdded = time(NULL);
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sName, int iDriveID)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::ltos(iPK_Users));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sName);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(iDriveID));
	ScreenHandlerBase::SCREEN_CDTrackCopy(PK_Screen, iPK_Users, sName, iDriveID);
}
//-----------------------------------------------------------------------------------------------------
string MediaFileBrowserOptions::HumanReadable()
{
	string sResult = m_pOrbiter->m_mapPK_MediaType_Description[ m_PK_MediaType ] + " : " 
		+ m_pOrbiter->m_mapPK_AttributeType_Description[ m_PK_AttributeType_Sort ];

	string sFilter;
	if( m_listPK_Attribute_Description.size() )
		sFilter = m_listPK_Attribute_Description.front().second;

	bool bHide = sFilter.size() ? false : true;
	DesignObj_Orbiter *pObj_Back = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFileBrowserBack_CONST));
	if( pObj_Back && pObj_Back->m_bHidden != bHide )
	{
		NeedToRender render2( m_pOrbiter, "ScreenHandler::HumanReadable" );
		m_pOrbiter->CMD_Show_Object( pObj_Back->m_ObjectID,0,"","", bHide ? "0" : "1" );
	}
	return sResult + sFilter;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileList_Music_Movies_Video(long PK_Screen)
{
	if( m_pOrbiter->UsesUIVersion2()==false )
	{
		ScreenHandlerBase::SCREEN_FileList_Music_Movies_Video(PK_Screen);
		return;
	}

	ScreenHandlerBase::SCREEN_FileList_Music_Movies_Video(PK_Screen);

	SetMediaSortFilterSelectedObjects();
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_ObjectSelected,	new ObjectInfoBackData());
	RegisterCallBack(cbOnRenderScreen, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_Render, new RenderScreenCallBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_DatagridSelected, new DatagridCellBackData());

	return;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SetMediaSortFilterSelectedObjects()
{
	string sMediaType = StringUtils::itos(mediaFileBrowserOptions.m_PK_MediaType);
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaType_CONST)), mediaFileBrowserOptions.m_PK_MediaType );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) "." + sMediaType + ".0"), mediaFileBrowserOptions.m_sPK_MediaSubType );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST) "." + sMediaType + ".0"), mediaFileBrowserOptions.m_sPK_FileFormat );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) "." + sMediaType + ".0"), mediaFileBrowserOptions.m_sPK_Attribute_Genres );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) "." + sMediaType + ".0"), mediaFileBrowserOptions.m_PK_AttributeType_Sort );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) "." + sMediaType + ".0"), mediaFileBrowserOptions.m_sSources );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PrivateMedia_CONST)), mediaFileBrowserOptions.m_sPK_Users_Private );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_RatingsByUser_CONST)), mediaFileBrowserOptions.m_PK_Users );
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_Render(CallBackData *pData)
{
	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_objFileBrowserHeader_CONST) );
	if( pObj && pObj->m_vectDesignObjText.size() )
	{
		DesignObjText *pText = pObj->m_vectDesignObjText[0];
		pText->m_sText = mediaFileBrowserOptions.HumanReadable();
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	PLUTO_SAFETY_LOCK(vm, m_pOrbiter->m_ScreenMutex);

	if(	pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_dgFileList2_CONST	|| pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_dgFileList2_Pics_CONST || pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objCDCover_CONST ) // todo
	{
		DataGridCell *pCell_Pic=NULL,*pCell_List=NULL;
		if( mediaFileBrowserOptions.m_pObj_PicGrid->DataGridTable_Get() )
			pCell_Pic = mediaFileBrowserOptions.m_pObj_PicGrid->DataGridTable_Get()->GetData(mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedColumn + mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurCol,mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedRow + mediaFileBrowserOptions.m_pObj_PicGrid->m_GridCurRow);
		if( mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get() )
			pCell_List = mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get()->GetData(0,mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow + mediaFileBrowserOptions.m_pObj_ListGrid->m_GridCurRow);
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::MediaBrowser_ObjectSelected sel grid pic %p list %p pich %d,%d  listh %d,%d",
			pCell_Pic,pCell_List,mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedColumn,mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedRow,
			0,mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow);
#endif

		if( !pCell_List )
			return false; // Shouldn't happen

		if( !pCell_List->m_Value || strstr(pCell_List->m_Value,"\t!D")!=NULL )  // If it contains this, the user is going into a subdirectory
		{
			// It's a sub directory.  Update the source and refresh the page
			mediaFileBrowserOptions.m_sSources = pCell_List->m_Value ? pCell_List->m_Value : "";
			MediaBrowser_Render(NULL);
			m_pOrbiter->CMD_Refresh("*");
			return false;
		}

		GetAttributesForMediaFile(pCell_List->m_Value);

		m_pOrbiter->CMD_Remove_Popup("","coverart");

		if( pCell_List->m_Value[0]=='!' && pCell_List->m_Value[1]=='A' )
		{
			SelectedAttributeCell(pCell_List);
			return false;
		}

		DesignObj_Orbiter *pObj_Play = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_butFBSF_Play_CONST) );
		if( !pObj_Play || !pObj_Play->m_pParentObject )
			return false; // Shouldn't happen

		mediaFileBrowserOptions.m_sSelectedFile = pCell_List->m_Value;
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,mediaFileBrowserOptions.m_sSelectedFile);
		m_pOrbiter->CMD_Show_Popup(pObj_Play->m_pParentObject->m_ObjectID,10,10,"","filedetails",false,false);

		if( pCell_Pic && pCell_Pic->m_pGraphic && pCell_Pic->m_pGraphic->m_pGraphicData )
			m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",
				pCell_Pic->m_pGraphic->m_pGraphicData,
				int(pCell_Pic->m_pGraphic->m_GraphicLength),"0");
		else if( pCell_Pic && pCell_Pic->m_pGraphicData )
			m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",
				pCell_Pic->m_pGraphicData,
				pCell_Pic->m_GraphicLength,"0");
		else
			m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",
				NULL,
				0,"0");

		DesignObj_Orbiter *pParentObject = dynamic_cast<DesignObj_Orbiter *>(pObj_Play->m_pParentObject);
		if(NULL != pParentObject)
			m_pOrbiter->Renderer()->RenderObjectAsync(pParentObject);

		m_pOrbiter->m_pObj_Highlighted = pObj_Play;
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse(pObj_Play->m_pParentObject->m_rPosition + pObj_Play->m_pPopupPoint );
		m_pOrbiter->m_pMouseBehavior->SetMousePosition(pObj_Play);
#endif
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Close_CONST )
	{
		m_pOrbiter->m_pObj_Highlighted = mediaFileBrowserOptions.m_pObj_ListGrid;
		m_pOrbiter->CMD_Remove_Popup("","filedetails");

		DesignObj_Orbiter *pObj_CoverArt = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(DESIGNOBJ_objCDCover_CONST) );
		if(NULL != pObj_CoverArt)
			m_pOrbiter->Renderer()->RemoveGraphic(pObj_CoverArt->GenerateObjectHash(pObj_CoverArt->m_pPopupPoint, false));


#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Play_CONST )
	{
		if( !mediaFileBrowserOptions.m_pObj_ListGrid->DataGridTable_Get() )
			return false; //shouldn't happen

g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::MediaBrowser_ObjectSelected Play Media higlighted %s",mediaFileBrowserOptions.m_sSelectedFile.c_str());

		m_pOrbiter->CMD_Remove_Popup("","filedetails");
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
		DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			0,mediaFileBrowserOptions.m_sSelectedFile,0,0,StringUtils::itos( m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),false,0);
		m_pOrbiter->SendCommand(CMD_MH_Play_Media);
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFileBrowserBack_CONST )
	{
		if( mediaFileBrowserOptions.m_listPK_Attribute_Description.size() )
		{
			mediaFileBrowserOptions.m_listPK_Attribute_Description.pop_front();
			if( mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.size() )
			{
				mediaFileBrowserOptions.m_PK_AttributeType_Sort = mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.front();
				mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.pop_front();
			}
		}
		else
			mediaFileBrowserOptions.ClearAll(mediaFileBrowserOptions.m_PK_MediaType,m_pOrbiter->m_pScreenHistory_Current->PK_Screen(),
				m_pOrbiter->m_mapPK_MediaType_PK_Attribute_Sort[mediaFileBrowserOptions.m_PK_MediaType]);

		MediaBrowser_Render(NULL);
		m_pOrbiter->CMD_Refresh("*");
		return false;
	}
	else if( pObjectInfoData->m_pObj && pObjectInfoData->m_pObj->m_pParentObject )
	{
		bool bChangedSortFilter=false;
		switch( pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID )
		{
		case DESIGNOBJ_popFBSF_Genres_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_Attribute_Genres,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_PK_FileFormat_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_FileFormat,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_MediaType_CONST:
			{
				mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_MediaType);
				m_pOrbiter->CMD_Show_File_List(mediaFileBrowserOptions.m_PK_MediaType);
			}
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_PrivateMedia_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_Users_Private,false);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_RatingsByUser_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_Users);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_Sort_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_AttributeType_Sort);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_MediaSource_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sSources,true);
			bChangedSortFilter=true;
			break;
		case DESIGNOBJ_popFBSF_PK_MediaSubType_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_MediaSubType,true);
			bChangedSortFilter=true;
			break;
		};
		if( bChangedSortFilter )
		{
			SetMediaSortFilterSelectedObjects();
			m_pOrbiter->Renderer()->RenderObjectAsync(pObjectInfoData->m_pObj);  // We will be changing the selected state
			m_pOrbiter->CMD_Refresh("*");
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_nPK_Datagrid==DATAGRID_Media_Attributes_For_File_CONST )
	{
		SelectedAttributeCell(pCellInfoData->m_pDataGridCell);
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
		m_pOrbiter->CMD_Remove_Popup("","filedetails");
		m_pOrbiter->CMD_Remove_Popup("","coverart");
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SelectedAttributeCell(DataGridCell *pCell)
{
	mediaFileBrowserOptions.m_listPK_AttributeType_Sort_Prior.push_front(mediaFileBrowserOptions.m_PK_AttributeType_Sort);
	mediaFileBrowserOptions.m_listPK_Attribute_Description.push_front( make_pair<int,string> (atoi(&pCell->m_Value[2]),pCell->m_Text ? pCell->m_Text : "*unknown attribute*") );

	// If we're browing a collection of attributes, say actors, when we choose
	// one we no longer want to browsing (or sorting by) that same attribute (actors)
	// we now want to browse by the titles with that attribute.  So we will switch
	// the sort.  Normally it goes to 'Title'.  However for Artists it goes to Albums
	// and for media without titles, like pictures, it goes to filename (which is 0)
	if( (mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_pluto_CD_CONST || mediaFileBrowserOptions.m_PK_MediaType==MEDIATYPE_pluto_StoredAudio_CONST)
		&& mediaFileBrowserOptions.m_PK_AttributeType_Sort==ATTRIBUTETYPE_Performer_CONST )
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Album_CONST;
	else
	{
		switch( mediaFileBrowserOptions.m_PK_AttributeType_Sort )
		{
		case ATTRIBUTETYPE_Director_CONST:
		case ATTRIBUTETYPE_Performer_CONST:
		case ATTRIBUTETYPE_Album_CONST:
		case ATTRIBUTETYPE_Title_CONST:
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = ATTRIBUTETYPE_Title_CONST;
			break;

		default:
			mediaFileBrowserOptions.m_PK_AttributeType_Sort = 0;  // Show files
			break;
		}

	}
	MediaBrowser_Render(NULL);
	m_pOrbiter->CMD_Refresh("*");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GetAttributesForMediaFile(const char *pFilename)
{
	m_mapKeywords.clear();
	string sResult;
	DCE::CMD_Get_Attributes_For_Media CMD_Get_Attributes_For_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
		pFilename,"",&sResult);
	if( m_pOrbiter->SendCommand(CMD_Get_Attributes_For_Media) )
	{
		string::size_type pos=0;
		while(pos<sResult.size())
		{
			string s = StringUtils::Tokenize(sResult,"\t",pos);
			m_mapKeywords[s] = StringUtils::Tokenize(sResult,"\t",pos);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaFileBrowserOptions::SelectArrays(DesignObj_Orbiter *pObj,string &sValues)
{
	if( !pObj )
		return; // Shouldn't happen

	for( DesignObj_DataList::iterator iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		pDesignObj_Orbiter->m_bDontResetState=true;
		string sArrayValue = pDesignObj_Orbiter->GetArrayValue();
		m_mapObjectsValues[ make_pair<int,string> (pDesignObj_Orbiter->m_iBaseObjectID,sArrayValue) ] = pDesignObj_Orbiter;
		if( sArrayValue.size()==0 || atoi(sArrayValue.c_str())==0 )
			pDesignObj_Orbiter->m_GraphicToDisplay_set(sValues.size()==0 ? GRAPHIC_SELECTED : GRAPHIC_NORMAL,false,true);
		else if( ("," + sValues + ",").find( "," + sArrayValue + "," )!=string::npos )
			pDesignObj_Orbiter->m_GraphicToDisplay_set(GRAPHIC_SELECTED,false,true);
		else
			pDesignObj_Orbiter->m_GraphicToDisplay_set(GRAPHIC_NORMAL,false,true);
	}
}
//-----------------------------------------------------------------------------------------------------
void MediaFileBrowserOptions::SelectArrays(DesignObj_Orbiter *pObj,int &iValue)
{
	if( !pObj )
		return; // Shouldn't happen

	for( DesignObj_DataList::iterator iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
	{
		DesignObj_Orbiter *pDesignObj_Orbiter=( DesignObj_Orbiter * )*iHao;
		pDesignObj_Orbiter->m_bDontResetState=true;
		string sArrayValue = pDesignObj_Orbiter->GetArrayValue();
		m_mapObjectsValues[ make_pair<int,string> (pDesignObj_Orbiter->m_iBaseObjectID,sArrayValue) ] = pDesignObj_Orbiter;
		if( atoi(sArrayValue.c_str())==iValue )
			pDesignObj_Orbiter->m_GraphicToDisplay_set(GRAPHIC_SELECTED,false,true);
		else
			pDesignObj_Orbiter->m_GraphicToDisplay_set(GRAPHIC_NORMAL,false,true);
	}
}

void MediaFileBrowserOptions::SelectedArray(DesignObj_Orbiter *pObj,string &sValues,bool bTreatZeroAsAll)
{
	if( !pObj )
		return; // Shouldn't happen
	string sArrayValue = pObj->GetArrayValue();
	string::size_type pos=0,prior=0;
	if( sArrayValue!="0" || !bTreatZeroAsAll )  // Don't bother if the user selected the 'all'
	{
		while(pos<sValues.size())
		{
			string sToken = StringUtils::Tokenize(sValues,",",pos);
			if( sToken==sArrayValue )
			{
				string sNewValue;
				if( prior==0 && pos>=sValues.size() )
					sNewValue = "";
				else if( prior==0 )
					sNewValue = sValues.substr(pos);
				else if( pos>=sValues.size() )
					sNewValue = sValues.substr(0,prior-1);
				else
					sNewValue = sValues.substr(0,prior) + sValues.substr(pos);
				sValues = sNewValue;
				return;
			}
			prior=pos;
		}
	}

	if( bTreatZeroAsAll )
	{
		// Clear the 'all' unless the user selected all, in which case clear everything else
		map< pair<int,string>, DesignObj_Orbiter * >::iterator it = m_mapObjectsValues.find( make_pair<int,string> ( pObj->m_iBaseObjectID, "0" ) );
		if( it != m_mapObjectsValues.end() )
		{
			DesignObj_Orbiter *pObj_All = it->second;
			if( pObj_All==pObj )  // user selected all, clear everything else
			{
				pObj->m_GraphicToDisplay_set(GRAPHIC_SELECTED,false,true);
				for( DesignObj_DataList::iterator iHao=pObj->m_pParentObject->m_ChildObjects.begin(  ); iHao != pObj->m_pParentObject->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pObj_Child=( DesignObj_Orbiter * )*iHao;
					if( pObj_Child!=pObj )
					{
						pObj_Child->m_GraphicToDisplay_set(GRAPHIC_NORMAL,false,true);
						m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Child);
					}
				}
				sValues="";
				return;
			}
			else  // User selected something besides 'all'
			{
				pObj_All->m_GraphicToDisplay_set(GRAPHIC_NORMAL,false,true);
				m_pOrbiter->Renderer()->RenderObjectAsync(pObj_All);
			}
		}
	}

	pObj->m_GraphicToDisplay_set(GRAPHIC_SELECTED,false,true);

	if( sValues.size() )
		sValues += ",";
	sValues += sArrayValue;
int k=2;
}

void MediaFileBrowserOptions::SelectedArray(DesignObj_Orbiter *pObj,int &iValue)
{
	if( !pObj )
		return; // Shouldn't happen

	string sValue = pObj->GetArrayValue();
	int iValue_New = atoi(sValue.c_str());
	if( iValue==iValue_New )
		return; // User just chose the same thing

	// Find the old one if it's changed
	map< pair<int,string>, DesignObj_Orbiter * >::iterator it= m_mapObjectsValues.find( make_pair<int,string> (pObj->m_iBaseObjectID,StringUtils::itos(iValue)) );
	if( it!=m_mapObjectsValues.end() )
	{
		DesignObj_Orbiter *pObj_PriorSelected = it->second;
		pObj_PriorSelected->m_GraphicToDisplay_set(GRAPHIC_NORMAL,false,true);
		m_pOrbiter->Renderer()->RenderObjectAsync(pObj_PriorSelected);  // We will be changing the selected state
	}

	pObj->m_GraphicToDisplay_set(GRAPHIC_SELECTED,false,true);
	iValue = iValue_New;
}

//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription, int iPK_PnpQueue)
{
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sDescription);
	ScreenHandlerBase::SCREEN_NewPhoneDetected(PK_Screen, sMac_address, sDescription, iPK_PnpQueue);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_WhatModelMobileOrbiter(long PK_Screen, int iPK_Users, string sMac_address)
{
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMac_address);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(iPK_Users));
	ScreenHandlerBase::SCREEN_WhatModelMobileOrbiter(PK_Screen, iPK_Users, sMac_address);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Pick_Room_For_Device(long PK_Screen, int iPK_Device,
												   string sDescription, string sComments)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sComments);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, StringUtils::ltos(iPK_Device));
	GotoScreen(SCREEN_Pick_Room_For_Device_CONST,StringUtils::ltos(iPK_Device),true,false);
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


	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sAlerts, TEXT_Alerts_Placeholder_CONST);
	ScreenHandlerBase::SCREEN_ModeChanged(PK_Screen, sPK_HouseMode, sHouseModeTime, sExitDelay, sAlerts);
	if(sExitDelay != "")
		m_pOrbiter->CMD_Select_Object(sPK_DesignObj, StringUtils::itos(DESIGNOBJ_mnuModeChanged_CONST), sExitDelay);  // Do this last since the above resets the selected object
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CurrentlyActiveRemote(long PK_Screen)
{
	if( !m_pOrbiter->UsesUIVersion2() &&  // TODO -- Is this right?????  When we say go to remote, we always want the chapters, right??
		m_pOrbiter->m_bIsOSD && m_pOrbiter->m_iPK_Screen_RemoteOSD && m_pOrbiter->m_iLocation_Initial==m_pOrbiter->m_pLocationInfo->iLocation)  // If we've changed locations, we're not the OSD anymore
		m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_RemoteOSD);
	else if( m_pOrbiter->m_iPK_Screen_Remote )
		m_pOrbiter->CMD_Goto_Screen("",m_pOrbiter->m_iPK_Screen_Remote);
	else
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::SCREEN_CurrentlyActiveRemote no remote");
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Computing(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_Computing(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_DatagridSelected, new DatagridCellBackData());

	string sActiveApplication_Description;
	if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
	{
		string sPK_DesignObj_ActiveApp_OSD,sPK_DesignObj_ActiveApp_Remote;

		DCE::CMD_Get_Active_Application CMD_Get_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
			&sPK_DesignObj_ActiveApp_OSD,&sPK_DesignObj_ActiveApp_Remote,&m_sActiveApplication_Description,&m_sActiveApplication_Window);
		m_pOrbiter->SendCommand(CMD_Get_Active_Application);
		m_PK_DesignObj_ActiveApp_OSD = atoi(sPK_DesignObj_ActiveApp_OSD.c_str());
		m_PK_DesignObj_ActiveApp_Remote = atoi(sPK_DesignObj_ActiveApp_Remote.c_str());
	}
	else
	{
		m_sActiveApplication_Description = m_pOrbiter->m_sActiveApplication_Description;
		m_sActiveApplication_Window = m_pOrbiter->m_sActiveApplication_Window;
		m_PK_DesignObj_ActiveApp_OSD = m_pOrbiter->m_PK_DesignObj_ActiveApp_OSD;
		m_PK_DesignObj_ActiveApp_Remote = m_pOrbiter->m_PK_DesignObj_ActiveApp_Remote;
	}

	string sOnOff = m_sActiveApplication_Description.size() ? "1" : "0";
	m_pOrbiter->CMD_Show_Object( NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID + "." TOSTRING(DESIGNOBJ_butResumeControl_CONST),0,"","",sOnOff);
	m_pOrbiter->CMD_Show_Object(NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID + "." TOSTRING(DESIGNOBJ_objExitAppOnOSD_CONST),0,"","",sOnOff);

	m_pOrbiter->CMD_Set_Text(NeedToRender::m_pScreenHistory_get()->GetObj()->m_ObjectID,m_sActiveApplication_Description,TEXT_STATUS_CONST);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_GenericAppFullScreen(long PK_Screen)
{
	ScreenHandlerBase::SCREEN_GenericAppFullScreen(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Computing_ObjectSelected, new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Computing_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_objExitAppOnOSD_CONST )
	{
		DCE::CMD_Kill_Application CMD_Kill_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_AppServer,"generic-app",true);
		m_pOrbiter->SendCommand(CMD_Kill_Application);

		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
		{
			DCE::CMD_Set_Active_Application CMD_Set_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				"","","","");
			DCE::CMD_Goto_Screen CMD_Goto_Screen(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,"",SCREEN_Main_CONST);
			CMD_Set_Active_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_Screen.m_pMessage);
			m_pOrbiter->SendCommand(CMD_Set_Active_Application);
		}
		else
		{
			m_pOrbiter->m_sActiveApplication_Description = "";
			m_pOrbiter->m_sActiveApplication_Window = "";
		}
		m_pOrbiter->CMD_Goto_Screen("",SCREEN_Main_CONST);
	}
	else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResumeControl_CONST )
	{
		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )
		{
			DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				0,StringUtils::itos(m_PK_DesignObj_ActiveApp_OSD),"","",false,true);
			m_pOrbiter->SendCommand(CMD_Goto_DesignObj);
			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(m_PK_DesignObj_ActiveApp_Remote),"","",false,false);

			if(m_sActiveApplication_Window != "")
			{
				DCE::CMD_Activate_Window cmd_Activate_Window(
					m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
					m_sActiveApplication_Window);
				m_pOrbiter->SendCommand(cmd_Activate_Window);
			}
		}
		else
		{
			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(m_PK_DesignObj_ActiveApp_OSD),"","",false,false);
			m_pOrbiter->CMD_Activate_Window(m_sActiveApplication_Window);
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Computing_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_sValue.size() && m_pOrbiter->m_pLocationInfo )
	{
		string::size_type pos=0;
		string sActiveApplication_Window = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		int PK_DesignObj_Remote = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos).c_str());
		int PK_DesignObj_OSD = atoi(StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos).c_str());
		if( !PK_DesignObj_Remote )
			PK_DesignObj_Remote = DESIGNOBJ_mnuGenericAppController_CONST;
		if( !PK_DesignObj_OSD )
			PK_DesignObj_OSD=DESIGNOBJ_generic_app_full_screen_CONST;
		string sActiveApplication_Description = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		string sBinary = StringUtils::Tokenize(pCellInfoData->m_sValue,"\t",pos);
		string sArguments = pos<pCellInfoData->m_sValue.size() ? pCellInfoData->m_sValue.substr(pos) : "";

		string sMessageOnComplete = "0 " + StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter) + " 1 " + TOSTRING(COMMAND_Set_Active_Application_CONST) +
			" " + TOSTRING(COMMANDPARAMETER_Name_CONST) + " \"\" " + TOSTRING(COMMANDPARAMETER_Identifier_CONST) + " \"\" " +
			TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) + " 0 " + TOSTRING(COMMANDPARAMETER_PK_DesignObj_CurrentScreen_CONST) + " 0"
			" & 0 " + StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter) + 
			" 1 4 16 " + StringUtils::itos(PK_DesignObj_OSD);

		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
			sMessageOnComplete += " & 0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " 1 " + TOSTRING(COMMAND_Set_Active_Application_CONST) +
			" " + TOSTRING(COMMANDPARAMETER_Name_CONST) + " \"\" " + TOSTRING(COMMANDPARAMETER_Identifier_CONST) + " \"\" " +
			TOSTRING(COMMANDPARAMETER_PK_DesignObj_CONST) + " 0 " + TOSTRING(COMMANDPARAMETER_PK_DesignObj_CurrentScreen_CONST) + " 0"
			" & 0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + 
			" 1 4 16 " + StringUtils::itos(PK_DesignObj_Remote);

		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_AppServer,
			sBinary,"generic-app",sArguments,sMessageOnComplete,sMessageOnComplete,false,false,true,false);
		
		if( m_pOrbiter->m_dwPK_Device != m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter )  // Not us
		{
			//DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,0,
			//	StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
			//CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

			DCE::SCREEN_GenericAppFullScreen screen_GenericAppFullScreen(m_pOrbiter->m_dwPK_Device,
				m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(screen_GenericAppFullScreen.m_pMessage);

			DCE::CMD_Set_Active_Application CMD_Set_Active_Application(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				StringUtils::itos(PK_DesignObj_OSD),StringUtils::itos(PK_DesignObj_Remote),sActiveApplication_Description,sActiveApplication_Window);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Active_Application.m_pMessage);

			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(PK_DesignObj_Remote),"","",false,false);

			DCE::CMD_Activate_Window cmd_Activate_Window(
				m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_pLocationInfo->m_dwPK_Device_Orbiter,
				sActiveApplication_Window);
			m_pOrbiter->SendCommand(cmd_Activate_Window);
		}
		else
		{
			m_pOrbiter->CMD_Goto_DesignObj(0,StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
			m_pOrbiter->m_sActiveApplication_Window = sActiveApplication_Window;
			m_pOrbiter->m_sActiveApplication_Description = sActiveApplication_Description;
			m_pOrbiter->m_PK_DesignObj_ActiveApp_OSD = PK_DesignObj_OSD;
			m_pOrbiter->m_PK_DesignObj_ActiveApp_Remote = PK_DesignObj_Remote;
			m_pOrbiter->CMD_Activate_Window(sActiveApplication_Window);
		}
		m_pOrbiter->SendCommand(CMD_Spawn_Application);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors)
{
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Cannot_play_media_CONST) + "%>" + sErrors);
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
	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Checking_drive_CONST) + "%>", false, "20",true,
		"Eject","0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_MediaPlugIn) + " 1 " + StringUtils::itos(COMMAND_Eject_Disk_CONST));
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
void ScreenHandler::SCREEN_DialogAskToResume(long PK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, 
					string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo)
{
	string sMessageToResume = sPK_Device_From + " " + sPK_Device_MediaSource
		+ " 1 " + StringUtils::itos(COMMAND_Set_Media_Position_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_StreamID_CONST) + " " + sStreamID_String + " "
		+ StringUtils::itos(COMMANDPARAMETER_MediaPosition_CONST) + " \"" + sPosition + "\"";

	string sMessageToGoToRemote = sPK_Device_From + " " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device)
		+ " 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::itos(iPK_Screen_GoTo);

	string sMessageToSetPreference = StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + sPK_Device_From
		+ " 1 " + StringUtils::itos(COMMAND_Set_Auto_Resume_Options_CONST) + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_Users_CONST) + " " + sUsers + " "
		+ StringUtils::itos(COMMANDPARAMETER_PK_MediaType_CONST) + " " + sPK_MediaType_String + " "
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
	string sOption3, string sMessage3, string sOption4, string sMessage4,string sID)
{
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::DisplayMessageOnOrbiter screen %d message %s option 1 %s / %s 2: %s / %s",
		PK_Screen,sMessage.c_str(),sOption1.c_str(),sMessage1.c_str(),sOption2.c_str(),sMessage2.c_str());

	NeedToRender render2( m_pOrbiter, "ScreenHandler::DisplayMessageOnOrbiter" );  // Redraw anything that was changed by this command
	int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
	if( !PK_DesignObj )
		PK_DesignObj = PK_Screen ? PK_Screen : DESIGNOBJ_mnuPopupMessage_CONST;  // Allow the user to pass in a design obj to use instead
	string sPK_DesignObj = StringUtils::ltos(PK_DesignObj);

	m_pOrbiter->CMD_Set_Timeout(sPK_DesignObj, sTimeout);

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Text_CONST, sMessage);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_1_CONST, sOption1);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_2_CONST, sOption2);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_3_CONST, sOption3);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Display_Message_Button_4_CONST, sOption4);

	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, sID, "", false, bCantGoBack );

	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, "<%=" + StringUtils::ltos(VARIABLE_Display_Message_Text_CONST) + "%>", TEXT_STATUS_CONST);

	if(bPromptToResetRouter)
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );

	if( sOption1.size()==0 && sOption2.size()==0 && sOption3.size()==0 && sOption4.size()==0 )
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, "0 -300 1 " TOSTRING(COMMAND_Go_back_CONST) " " TOSTRING(COMMANDPARAMETER_Force_CONST) " 1");
		return;
	}

	if(sOption1.size() )
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_1_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, sMessage1);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", sOption1.size() ? "1" : "0" );

	if(sOption2.size())
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_2_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_2_CONST, sMessage2);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 0, "", "", sOption2.size() ? "1" : "0" );

	if(sOption3.size())
	{
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_3_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_3_CONST, sMessage3);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 0, "", "", sOption3.size() ? "1" : "0" );

	if(sOption4.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 
			"<%=" + StringUtils::ltos(VARIABLE_Display_Message_Button_4_CONST) + "%>", TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_4_CONST, sMessage4);
	}
	m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", sOption4.size() ? "1" : "0" );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sPhoneName);
	DisplayMessageOnOrbiter(PK_Screen, sInstructions, true);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine,
	string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer)
{
	string sYesMessage =
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + sPK_DeviceFrom  + " " +
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Send_File_To_Phone_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_Mac_address_CONST) + " \"" + sMacAddress + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Command_Line_CONST) + " \"" + sCommandLine + "\"" + " " +
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
// Create some 'choose provider stages' to keep track of where we are
#define CPS_CONFIRM					1
#define CPS_CHOOSE_SOURCE			2
#define CPS_GETTING_USERNAME		3
#define CPS_GETTING_PASSWORD		4
#define CPS_GETTING_PROVIDER_LIST	5
#define CPS_PROMPTING_PROVIDER		6
#define CPS_GETTING_DEVICE_LIST		7
#define CPS_PROMPTING_DEVICE		8
#define CPS_GETTING_PACKAGE_LIST	9
#define CPS_PROMPTING_PACKAGE		10
#define CPS_GETTING_LINEUP_LIST		11
#define CPS_PROMPTING_LINEUP		12

void ScreenHandler::SCREEN_Choose_Provider_for_Device(long PK_Screen, int iPK_Device, string sText, string sDescription)
{
	if( iPK_Device==0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ScreenHandler::SCREEN_Choose_Provider_for_Device Device no device");
		return;
	}
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::SCREEN_Choose_Provider_for_Device Device %d",iPK_Device);
#endif

	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_Device));
	ScreenHandlerBase::SCREEN_Choose_Provider_for_Device(PK_Screen, iPK_Device,sText,sDescription);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::ChooseProvider_ObjectSelected, new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::ChooseProvider_DatagridSelected, new DatagridCellBackData());
	RegisterCallBack(cbMessageIntercepted, (ScreenHandlerCallBack) &ScreenHandler::ChooseProvider_Intercepted, new MsgInterceptorCellBackData());
	m_iStage = CPS_CONFIRM;

	string::size_type pos = 0;
	string sPK_DeviceTemplate_MediaType = StringUtils::Tokenize(sText,"\t",pos);
	string sPK_ProviderSource = StringUtils::Tokenize(sText,"\t",pos);
	
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, StringUtils::ltos(iPK_Device));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST,sText);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,sPK_DeviceTemplate_MediaType + "\t" + sPK_ProviderSource + "\t");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST,sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Username_CONST,"");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Password_CONST,"");
	ChooseProviderGetNextStage();
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ChooseProvider_Intercepted(CallBackData *pData)
{
	int PK_Device = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());

	string sValue; // If this is a set variable for VARIABLE_Execution_Result_CONST, then we got back a response and can continue
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted");
#endif
	MsgInterceptorCellBackData *pMsgInterceptorCellBackData = (MsgInterceptorCellBackData *) pData;
	if( pMsgInterceptorCellBackData->m_pMessage->m_dwMessage_Type==MESSAGETYPE_COMMAND && 
		pMsgInterceptorCellBackData->m_pMessage->m_dwID==COMMAND_Set_Variable_CONST )
	{
		map<long, string>::iterator it=pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.find(COMMANDPARAMETER_PK_Variable_CONST);
		if( it==pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.end() || atoi(it->second.c_str())!=VARIABLE_Execution_Result_CONST )
			return false;

		it=pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.find(COMMANDPARAMETER_Value_To_Assign_CONST);
		if( it==pMsgInterceptorCellBackData->m_pMessage->m_mapParameters.end() )
			return false;  // Should never happen
		sValue = it->second;
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted value is %s",sValue.c_str());
#endif
	}
	else
	{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted not set variable %d",pMsgInterceptorCellBackData->m_pMessage->m_dwID);
#endif
		return false;
	}

	DesignObj_Orbiter *pObj = m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgGenericDataGrid_CONST));
	if( !pObj )
		return false; // shouldn't happen
	
	DesignObj_DataGrid *pObj_Grid = dynamic_cast<DesignObj_DataGrid *> (pObj);
	pObj_Grid->m_bFlushOnScreen=false;
	pObj_Grid->m_GridCurRow = pObj_Grid->m_GridCurCol = 0;
	DataGridTable *pDataGridTable = new DataGridTable();
	pObj_Grid->DataGridTable_Set(pDataGridTable,0,0);
	DataGridCell *pCell;

	vector<string> vectLines;
	StringUtils::Tokenize(sValue,"\r\n",vectLines);
	vector<string>::iterator it = vectLines.begin();
	while(it!=vectLines.end() && *it!="OK")
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted skipping %s",it->c_str());
#endif
		++it;
	}

	int iRow=0;
	while( it!=vectLines.end() )
	{
		string::size_type pos = it->find('\t');
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted processing %s",it->c_str());
#endif
		if( pos == string::npos )
		{
			it++;
			continue; // Shouldn't happen
		}
		int ID = atoi( it->c_str() );
		string sValue = it->substr(pos+1);

		pCell = new DataGridCell(sValue,it->substr(0,pos));
		pObj_Grid->DataGridTable_Get()->SetData(0,iRow++,pCell);
		++it;
	}
	
	if(iRow==0)
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted no OK CALLING CMD_Remove_Screen_From_History with id: %s PK_Screen %d",m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
#endif
		DCE::CMD_Remove_Screen_From_History CMD_Remove_Screen_From_History(m_pOrbiter->m_dwPK_Device,DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
			StringUtils::itos(PK_Device),SCREEN_Choose_Provider_for_Device_CONST);
		m_pOrbiter->SendCommand(CMD_Remove_Screen_From_History);
		string sText = m_pOrbiter->m_mapTextString[TEXT_error_with_provider_CONST];
		DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,sText,"","errorprovider","1","","1");
		m_pOrbiter->ReceivedMessage(SCREEN_PopupMessage.m_pMessage);
		return false; // Keep processing it.
	}

	pDataGridTable->m_RowCount = pDataGridTable->GetRows();
	pDataGridTable->m_ColumnCount = pDataGridTable->GetCols();

	m_iStage++;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProvider_Intercepted row %d col %d stage %d",
		pDataGridTable->m_RowCount,pDataGridTable->m_ColumnCount,m_iStage);
#endif

	string sText;
	switch( m_iStage )
	{
	case CPS_PROMPTING_PROVIDER:
		sText = m_pOrbiter->m_mapTextString[TEXT_Choose_Provider_CONST];
		break;
	case CPS_PROMPTING_DEVICE:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Device_CONST];
		break;
	case CPS_PROMPTING_PACKAGE:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Package_CONST];
		break;
	case CPS_PROMPTING_LINEUP:
		sText = m_pOrbiter->m_mapTextString[TEXT_Provider_Lineup_CONST];
		break;
	}
	m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST),sText, TEXT_STATUS_CONST );
	m_pOrbiter->CMD_Set_Text( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) ".0.0." TOSTRING(DESIGNOBJ_butResponse1_CONST),m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], TEXT_STATUS_CONST );
	m_pOrbiter->GotoDesignObj( TOSTRING(DESIGNOBJ_mnuGenericDataGrid_CONST) );

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ChooseProvider_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID==DESIGNOBJ_mnuPopupMessage_CONST )
	{
		if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST && (m_iStage==CPS_CONFIRM || m_iStage==CPS_CHOOSE_SOURCE) )
		{
			m_iStage++;
			ChooseProviderGetNextStage();
			return true;
		}
		else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST && m_iStage==CPS_CONFIRM )
		{
			int PK_Device = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());
			DCE::CMD_Specify_Media_Provider CMD_Specify_Media_Provider(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,PK_Device,"NONE");
			m_pOrbiter->SendCommand(CMD_Specify_Media_Provider);
			m_pOrbiter->CMD_Go_back("","");
		}
	}
	else if( pObjectInfoData->m_pObj->m_pParentObject && pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID==DESIGNOBJ_mnuGenericKeyboard_CONST )
	{
		if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse1_CONST && (m_iStage==CPS_GETTING_USERNAME || m_iStage==CPS_GETTING_PASSWORD) )
		{
			if( m_iStage==CPS_GETTING_USERNAME )
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Username_CONST,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));
			else
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Password_CONST,m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST));

			m_iStage++;
			ChooseProviderGetNextStage();
			return true;
		}
		else if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butResponse2_CONST )
			m_pOrbiter->CMD_Go_back("","");
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::ChooseProvider_DatagridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_sValue.empty() )
		return false;

	string sPK_DeviceTemplate_MediaType,sPK_Provider_Source,sProvider,sPackage,sDevice,sLineup;
	string::size_type pos=0;

	string sArguments = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);

	sPK_DeviceTemplate_MediaType = StringUtils::Tokenize( sArguments, "\t", pos );
	sPK_Provider_Source = StringUtils::Tokenize( sArguments, "\t", pos );
	sProvider = StringUtils::Tokenize(sArguments,"\t",pos);
	sDevice = StringUtils::Tokenize(sArguments,"\t",pos);
	sPackage = StringUtils::Tokenize(sArguments,"\t",pos);
	sLineup = StringUtils::Tokenize(sArguments,"\t",pos);

	switch( m_iStage )
	{
	case CPS_PROMPTING_PROVIDER:
		sProvider = pCellInfoData->m_sValue;
		break;
	case CPS_PROMPTING_DEVICE:
		sDevice = pCellInfoData->m_sValue;
		break;
	case CPS_PROMPTING_PACKAGE:
		sPackage = pCellInfoData->m_sValue;
		break;
	case CPS_PROMPTING_LINEUP:
		sLineup = pCellInfoData->m_sValue;
		break;
	}

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST,sPK_DeviceTemplate_MediaType + "\t" + sPK_Provider_Source + "\t" + sProvider + "\t" + sDevice + "\t" + sPackage + "\t" + sLineup);
	ChooseProviderGetNextStage();

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::ChooseProviderGetNextStage()
{
	if( m_pOrbiter->m_pScreenHistory_Current->PK_Screen()!=SCREEN_Choose_Provider_for_Device_CONST || 
		atoi(m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str())==0 )
	{
		m_pOrbiter->m_pScreenHistory_Current->ScreenID( m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST) );  // Don't know why we keep losing the id
		g_pPlutoLogger->Write(LV_CRITICAL,"ScreenHandler::ChooseProviderGetNextStage recurring bug to fix -- screen id is: %s PK_Screen %d",
			m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
	}

	NeedToRender render( m_pOrbiter, "ScreenHandler::ChooseProviderGetNextStage()" );  // Redraw anything that was changed by this command
	string::size_type pos=0;
	string sTokens = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST);
	string sPK_DeviceTemplate_MediaType = StringUtils::Tokenize(sTokens,"\t",pos);
	string sPK_Provider_Source = StringUtils::Tokenize( sTokens, "\t", pos );
	string sDescription = StringUtils::Tokenize( sTokens, "\t", pos );
	string sComments = StringUtils::Tokenize( sTokens, "\t", pos );
	bool bRequireUsernamePassword = StringUtils::Tokenize( sTokens, "\t", pos )=="1";
	string sProviderCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sDeviceCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sPackageCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sLineupCommandLine = StringUtils::Tokenize( sTokens, "\t", pos );
	string sArguments = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);

	if( m_iStage==CPS_CONFIRM )
	{
		string sText = m_pOrbiter->m_mapTextString[TEXT_use_media_provider_CONST];
       	StringUtils::Replace( &sText, "<%=device%>", m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_3_CONST) );
		DisplayMessageOnOrbiter(0,
			sText, false,"0", false,
			m_pOrbiter->m_mapTextString[TEXT_YES_CONST]," ",
			m_pOrbiter->m_mapTextString[TEXT_NO_CONST]," ");
	}
	// If we don't already have a provider, or we do and there are some comments to show the user, we'll have this stage
	else if( m_iStage<=CPS_CHOOSE_SOURCE && (sPK_Provider_Source.empty() || sComments.empty()==false) )
	{
		m_iStage = CPS_CHOOSE_SOURCE;
// TODO -- need to prompt for the source if it's not provided		if( sPK_Provider_Source.empty()==false )
		{
			DisplayMessageOnOrbiter(0,
				sComments, false,"0", false,
				m_pOrbiter->m_mapTextString[TEXT_Ok_CONST]);
		}
// TODO -- need to prompt for the source if it's not provided		else
	}
	else if( bRequireUsernamePassword && m_iStage<=CPS_GETTING_USERNAME || m_iStage==CPS_GETTING_PASSWORD )
	{
		if( m_iStage<CPS_GETTING_USERNAME )
			m_iStage=CPS_GETTING_USERNAME;

		string sText = m_pOrbiter->m_mapTextString[ m_iStage==CPS_GETTING_USERNAME ? TEXT_What_is_your_username_CONST : TEXT_What_is_your_password_CONST ];

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,"");
		SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
			sText + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], "",
			"provider username/password", "0");

	}
	else if( sProviderCommandLine.empty()==false && m_iStage<=CPS_GETTING_PROVIDER_LIST )
	{
		m_iStage = CPS_GETTING_PROVIDER_LIST;
		SpawnProviderScript(sProviderCommandLine,sArguments);
	}
	else if( sDeviceCommandLine.empty()==false && m_iStage<=CPS_GETTING_DEVICE_LIST )
	{
		m_iStage = CPS_GETTING_DEVICE_LIST;
		SpawnProviderScript(sDeviceCommandLine,sArguments);
	}
	else if( sPackageCommandLine.empty()==false && m_iStage<=CPS_GETTING_PACKAGE_LIST )
	{
		m_iStage = CPS_GETTING_PACKAGE_LIST;
		SpawnProviderScript(sPackageCommandLine,sArguments);
	}
	else if( sLineupCommandLine.empty()==false && m_iStage<=CPS_GETTING_LINEUP_LIST )
	{
		m_iStage = CPS_GETTING_LINEUP_LIST;
		SpawnProviderScript(sLineupCommandLine,sArguments);
	}
	else
	{
		// We got everything.  Set the provider information for this device
		string sAllArguments = 
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Username_CONST) + "\t" +
			m_pOrbiter->m_mapVariable_Find(VARIABLE_Password_CONST) + "\t" +
			sArguments;

		int PK_Device = atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_PK_Device_1_CONST).c_str());
		DCE::CMD_Specify_Media_Provider CMD_Specify_Media_Provider(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,PK_Device,sAllArguments);
		m_pOrbiter->SendCommand(CMD_Specify_Media_Provider);
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::ChooseProviderGetNextStage CALLING CMD_Remove_Screen_From_History with id: %s PK_Screen %d",m_pOrbiter->m_pScreenHistory_Current->ScreenID().c_str(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
#endif
		DCE::CMD_Remove_Screen_From_History CMD_Remove_Screen_From_History(m_pOrbiter->m_dwPK_Device,DEVICETEMPLATE_VirtDev_All_Orbiters_CONST,
			StringUtils::itos(PK_Device),SCREEN_Choose_Provider_for_Device_CONST);
		m_pOrbiter->SendCommand(CMD_Remove_Screen_From_History);

		string sText = m_pOrbiter->m_mapTextString[TEXT_Media_provider_specified_CONST];
		DCE::SCREEN_PopupMessage SCREEN_PopupMessage(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device,sText,"","errorprovider","1","","1");
		m_pOrbiter->ReceivedMessage(SCREEN_PopupMessage.m_pMessage);
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SpawnProviderScript(string sCommandLine,string sArguments)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"ScreenHandler::SpawnProviderScript CMD %s args %s stage %d",sCommandLine.c_str(),sArguments.c_str(),m_iStage);
#endif
	DeviceData_Base *pDevice_Core = m_pOrbiter->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfTemplate(DEVICETEMPLATE_DCERouter_CONST);
	DeviceData_Base *pDevice_AppServer = pDevice_Core->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_App_Server_CONST);
	if( !pDevice_AppServer )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ScreenHandler::SpawnProviderScript no app server"); // shouldn't ever happen
		return; 
	}

	string sResult = "0 " + StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " 1 " TOSTRING(COMMAND_Set_Variable_CONST)
			" " TOSTRING(COMMANDPARAMETER_PK_Variable_CONST) " " TOSTRING(VARIABLE_Execution_Result_CONST)
			" " TOSTRING(COMMANDPARAMETER_Value_To_Assign_CONST) " ";

	string sAllArguments = 
		m_pOrbiter->m_mapVariable_Find(VARIABLE_Username_CONST) + "\t" +
		m_pOrbiter->m_mapVariable_Find(VARIABLE_Password_CONST) + "\t" +
		sArguments;

	DCE::CMD_Spawn_Application CMD_Spawn_Application(m_pOrbiter->m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,sCommandLine,"getprovider",
		sAllArguments,sResult + " \"Error running script\"",
		sResult + "\"<=spawn_log=>\"",false,false,false,false);
	m_pOrbiter->SendCommand(CMD_Spawn_Application);

	DisplayMessageOnOrbiter(0,m_pOrbiter->m_mapTextString[TEXT_please_wait_for_lookup_CONST],false,"0",false,m_pOrbiter->m_mapTextString[TEXT_CANCEL_CONST]);
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
void ScreenHandler::SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_CONST, StringUtils::ltos(iPK_Device));
	ScreenHandlerBase::SCREEN_SingleCameraViewOnly(PK_Screen, iPK_Device);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device)
{
	vector<string> vectDevices;
	StringUtils::Tokenize(sList_PK_Device, ",", vectDevices);

	if(vectDevices.size() == 4)
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, vectDevices[0]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_2_CONST, vectDevices[1]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_3_CONST, vectDevices[2]);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_4_CONST, vectDevices[3]);
		ScreenHandlerBase::SCREEN_QuadViewCameras(PK_Screen, sList_PK_Device);
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "The list of devices for SCREEN_QuadViewCameras is wrong: %s",
			sList_PK_Device.c_str());
	}
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue)
{
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), StringUtils::itos(iPK_PnpQueue), "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Get_Username_Password_For_Devices(long PK_Screen, bool bAlready_processed, string sDescription, int iPK_PnpQueue)
{
	string sText;
	int iPK_DeviceData;

	if( bAlready_processed )
	{
		sText = m_pOrbiter->m_mapTextString[TEXT_password_for_device_CONST];
		iPK_DeviceData = DEVICEDATA_Password_CONST;
	}
	else
	{
		sText = m_pOrbiter->m_mapTextString[TEXT_username_for_device_CONST];
		iPK_DeviceData = DEVICEDATA_Username_CONST;
	}

	StringUtils::Replace( &sText, "<%=device%>", sDescription );

	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_PnpQueue));
	SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
		sText + "|" + m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], 
		StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn) + " " +
		"1 " + StringUtils::ltos(COMMAND_Set_Pnp_Options_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_PnpQueue_CONST) + " " + StringUtils::itos(iPK_PnpQueue) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_DeviceData_CONST) + " " + StringUtils::itos(iPK_DeviceData) + " " +
		StringUtils::itos(COMMANDPARAMETER_Value_To_Assign_CONST) + " \"<%=17%>\"", 
		"PNP Device username/password", "0");

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::New_Phone_Enter_Number_DeviceConfigured(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if(
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butSaveMobileNumber_CONST	||
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objControllerHome_CONST		||
		pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objControllerBack_CONST
	)
	{
		string sName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST);
		m_pOrbiter->m_pEvent->SendMessage(
			new Message(
				m_pOrbiter->m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL,
				MESSAGETYPE_EVENT, EVENT_Device_Configured_CONST, 1, EVENTPARAMETER_Name_CONST, sName.c_str()
			)
		);
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sOptions, int iPK_PnpQueue)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, "");  // Camera sensors
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, sOptions);
	int iOptions = atoi(sOptions.c_str());
	if( iOptions & 1 ) // Need to do lights
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Lighting_Device_CONST));
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_lights_near_camera_CONST], TEXT_STATUS_CONST);
	}
	else
	{
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Security_Device_CONST));
		m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_Sensors_Viewed_CONST], TEXT_STATUS_CONST);
	}
	ScreenHandlerBase::SCREEN_Sensors_Viewed_By_Camera(PK_Screen, sOptions, iPK_PnpQueue);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::Sensors_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::Sensors_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_butSetVisibleSensors_CONST )
	{	
		string sExisting=m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_4_CONST);
		if( sExisting.empty()==false && sExisting[ sExisting.size()-1 ]!='|' )
			sExisting += "|";
		sExisting += m_pOrbiter->m_mapVariable_Find(VARIABLE_Datagrid_Input_CONST);

		m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, sExisting); 
		if( atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_2_CONST).c_str())==DEVICECATEGORY_Lighting_Device_CONST && atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_3_CONST).c_str()) & 2 )
		{
			m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, StringUtils::itos(DEVICECATEGORY_Security_Device_CONST));
			m_pOrbiter->CMD_Set_Text(TOSTRING(DESIGNOBJ_mnuSensorsViewedByCamera_CONST), m_pOrbiter->m_mapTextString[TEXT_Which_Sensors_Viewed_CONST], TEXT_STATUS_CONST);
			GotoScreen(SCREEN_Sensors_Viewed_By_Camera_CONST,m_pOrbiter->m_pScreenHistory_Current->ScreenID(),true,true);
		}
		else
		{
			StringUtils::Replace(&sExisting,"|","\t");  // Per CreateDevice::DoIt relations are tab separated
			DCE::CMD_Set_Pnp_Options CMD_Set_Pnp_Options(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_PlugAndPlayPlugIn,
				sExisting,DEVICEDATA_sPK_Device_Relations_For_Creat_CONST,atoi(m_pOrbiter->m_mapVariable_Find(VARIABLE_Misc_Data_1_CONST).c_str()));
			m_pOrbiter->SendCommand(CMD_Set_Pnp_Options);
			m_pOrbiter->CMD_Remove_Screen_From_History(m_pOrbiter->m_pScreenHistory_Current->ScreenID(),m_pOrbiter->m_pScreenHistory_Current->PK_Screen());
		}
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::BadGotoScreen(int PK_Screen)
{
	g_pPlutoLogger->Write(LV_CRITICAL,"ScreenHandler::BadGotoScreen %d",PK_Screen);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void ScreenHandler::SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device,
	string sPK_Device_AppServer)
{
	string sYesMessage =
		StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + sPK_Device_AppServer  + " " +
		StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Halt_Device_CONST) + " " +
		StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " \"" + StringUtils::ltos(iPK_Device) + "\"" + " " +
		StringUtils::itos(COMMANDPARAMETER_Force_CONST) + " \"R\"";

	DisplayMessageOnOrbiter(PK_Screen, "<%=T" + StringUtils::itos(TEXT_Download_finnished_CONST) + "%>",
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
/*virtual*/ void ScreenHandler::SCREEN_Add_Software(long PK_Screen)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_DesignObj_CurrentSecti_CONST, TOSTRING(DESIGNOBJ_butAVDevicesWizard_CONST));
	ScreenHandlerBase::SCREEN_Add_Software(PK_Screen);
	RegisterCallBack(cbObjectHighlighted, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_ObjectHighlighted,	new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_GridSelected, new DatagridCellBackData());
	RegisterCallBack(cbDataGridRendering, (ScreenHandlerCallBack) &ScreenHandler::AddSoftware_GridRendering,	new DatagridAcquiredBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_ObjectHighlighted(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;
	if( pObjectInfoData->m_pObj->m_iBaseObjectID==DESIGNOBJ_dgInstallableApps_CONST )
	{
		NeedToRender render2( m_pOrbiter, "ScreenHandler::AddSoftware_ObjectHighlighted" );  // Redraw anything that was changed by this command
		m_pOrbiter->Renderer()->RenderTextAsync( m_pOrbiter->FindText( (DesignObj_Orbiter *) pObjectInfoData->m_pObj->m_pParentObject,TEXT_selected_add_software_CONST ) );

//		DataGridCell *pCell = m_pOrbiter->GetDataGridHighlightCell( (DesignObj_DataGrid *) pObjectInfoData->m_pObj);
//		m_pOrbiter->CMD_Refresh("");
	}
	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_GridRendering(CallBackData *pData)
{
	// This is called every time a new section of the grid is to be rendered.  We want to find the child object for the 'virus free' check and hide it if it's virus free,
	// and also find the child object for the icon and assign it the picture associated with the cell.
	DatagridAcquiredBackData *pDatagridAcquiredBackData = (DatagridAcquiredBackData *) pData;  // Call back data containing relevant values for the grid/table being rendered

	// Iterate through all the cells
	for(MemoryDataTable::iterator it=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.begin();it!=pDatagridAcquiredBackData->m_pDataGridTable->m_MemoryDataTable.end();++it)
	{
		DataGridCell *pCell = it->second;
		pair<int,int> colRow = DataGridTable::CovertColRowType(it->first);  // Get the column/row for the cell

		// See if there is an object assigned for this column/row
		map< pair<int,int>, DesignObj_Orbiter *>::iterator itobj = pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.find( colRow );
		if( itobj!=pDatagridAcquiredBackData->m_pObj->m_mapChildDgObjects.end() )
		{
			DesignObj_Orbiter *pObj = itobj->second;  // This is the cell's object.
			DesignObj_DataList::iterator iHao;

			// Iterate through all the object's children
			for( iHao=pObj->m_ChildObjects.begin(  ); iHao != pObj->m_ChildObjects.end(  ); ++iHao )
			{
				DesignObj_Orbiter *pDesignObj_Orbiter = (DesignObj_Orbiter *)( *iHao );
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_Installed_Apps_Not_Virus_Free_CONST )
					pDesignObj_Orbiter->m_bHidden = pCell->m_mapAttributes_Find("Virus_Free")=="Yes";  // Hide this if it's virus fee
				if( pDesignObj_Orbiter->m_iBaseObjectID==DESIGNOBJ_Installed_Apps_Icon_CONST )
					m_pOrbiter->m_pOrbiterRenderer->UpdateObjectImage(pDesignObj_Orbiter->m_ObjectID, "PNG",
						pCell->m_pGraphicData, pCell->m_GraphicLength, "0");  // Store the icon, which is cell's picture
			}
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::AddSoftware_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
	if( pCellInfoData->m_pDataGridCell )
	{
		int PK_Software = atoi(pCellInfoData->m_pDataGridCell->GetValue());
		string sInstallation_Status = pCellInfoData->m_pDataGridCell->m_mapAttributes_Find("Installation_status");
		string sText,sCommand;
		if( PK_Software && sInstallation_Status!="Yes" )
		{
			sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_Add_Software_CONST];
			sCommand = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn)
				+ " 1 " + TOSTRING(COMMAND_Add_Software_CONST) + " " + TOSTRING(COMMANDPARAMETER_PK_Software_CONST)
				+ " " + pCellInfoData->m_pDataGridCell->GetValue() + " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " "
				+ StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector) + " " TOSTRING(COMMANDPARAMETER_TrueFalse_CONST) " 1";
		}
		else if( PK_Software )
		{
			sText = m_pOrbiter->m_mapTextString[TEXT_Confirm_delete_software_CONST];
			sCommand = StringUtils::itos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::itos(m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn)
				+ " 1 " + TOSTRING(COMMAND_Add_Software_CONST) + " " + TOSTRING(COMMANDPARAMETER_PK_Software_CONST)
				+ " " + StringUtils::itos(PK_Software*-1) + " " TOSTRING(COMMANDPARAMETER_PK_Device_CONST) " "
				+ StringUtils::itos(m_pOrbiter->m_pLocationInfo->m_dwPK_Device_MediaDirector) + " " TOSTRING(COMMANDPARAMETER_TrueFalse_CONST) " 0";
		}
		else
			return false; // No software was selected

		DataGridCell *pCell = pCellInfoData->m_pDesignObj_DataGrid->DataGridTable_Get() ? pCellInfoData->m_pDesignObj_DataGrid->DataGridTable_Get()->GetData( 0, pCellInfoData->m_Row ) : NULL;
		if( pCell )
			sText+="\n" + pCell->m_mapAttributes["Title"];
		DisplayMessageOnOrbiter(0,sText,false,"30",false,m_pOrbiter->m_mapTextString[TEXT_YES_CONST],sCommand,m_pOrbiter->m_mapTextString[TEXT_NO_CONST]);
	}

	return false; // Keep processing it
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_FileSave(long PK_Screen, string sCaption, string sCommand, bool bAdvanced_options)
{
	//the command to execute with the selected file
	m_sSaveFile_Command = sCommand;
	m_bSaveFile_Advanced_options = bAdvanced_options;

	if(!m_bSaveFile_CreatingFolder)
	{
		//get default ripping info
		string sMounterFolder;
		CMD_Get_Default_Ripping_Info cmd_CMD_Get_Default_Ripping_Info(m_pOrbiter->m_dwPK_Device,
			m_pOrbiter->m_dwPK_Device_MediaPlugIn, &m_pOrbiter->m_sDefaultRippingName,
			&sMounterFolder, &m_pOrbiter->m_nDefaultStorageDeviceForRipping, 
			&m_pOrbiter->m_sDefaultStorageDeviceForRippingName);
		m_pOrbiter->SendCommand(cmd_CMD_Get_Default_Ripping_Info);

		if(m_nSaveFile_PK_DeviceDrive == 0)
			m_nSaveFile_PK_DeviceDrive = m_pOrbiter->m_nDefaultStorageDeviceForRipping;

		if(m_sSaveFile_Drive == "")
			m_sSaveFile_Drive = m_pOrbiter->m_sDefaultStorageDeviceForRippingName; 

		if(m_sSaveFile_FileName == "")
			m_sSaveFile_FileName = m_pOrbiter->m_sDefaultRippingName;

		if(m_sSaveFile_MountedFolder == "")
			m_sSaveFile_MountedFolder = sMounterFolder;
	}

	//setup variables
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, m_sSaveFile_Drive);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, m_sSaveFile_RelativeFolder);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(m_nSaveFile_PK_DeviceDrive));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, FileUtils::ExcludeTrailingSlash(m_sSaveFile_MountedFolder) + "\nMT-1\nP");
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, m_sSaveFile_FileName);
	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sCaption, TEXT_STATUS_CONST);
	ScreenHandlerBase::SCREEN_FileSave(PK_Screen, sCaption, sCommand, bAdvanced_options);

	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::FileSave_ObjectSelected,	
		new ObjectInfoBackData());
	RegisterCallBack(cbDataGridSelected, (ScreenHandlerCallBack) &ScreenHandler::FileSave_GridSelected, 
		new DatagridCellBackData());

	if(m_bSaveFile_CreatingFolder)
	{
		SaveFile_GotoChooseFolderDesignObj();
		m_bSaveFile_CreatingFolder = false;
	}
	else
	{
		m_pOrbiter->CMD_Show_Object(StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)) + ".0.0." + StringUtils::ltos(DESIGNOBJ_butChooseDrive_CONST), 0, "", "", 
			m_bSaveFile_Advanced_options ? "1" : "0");
	}
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileSave_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = dynamic_cast<ObjectInfoBackData *>(pData);
	
	if(NULL != pObjectInfoData)
	{
		if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseDrive_CONST)
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChoose_CONST)
			{
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, m_sSaveFile_Drive);
				m_pOrbiter->CMD_Go_back("", "");
				return true;
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseFolder_CONST)
		{
			if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChoose_CONST)
			{
				SaveFile_SendCommand();
				return true;
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butCreateDir_CONST)
			{
				string sParentFolder = m_pOrbiter->m_mapVariable_Find(VARIABLE_Path_CONST) + "/";
				string sNewFolder = "<%=" + StringUtils::ltos(VARIABLE_Seek_Value_CONST) + "%>";
				m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, "");

				int nAppServer = m_pOrbiter->TranslateVirtualDevice(DEVICETEMPLATE_VirtDev_AppServer_CONST);
				m_bSaveFile_CreatingFolder = true;

				SCREEN_GenericKeyboard(SCREEN_GenericKeyboard_CONST, 
					m_pOrbiter->m_mapTextString[TEXT_type_dir_name_CONST] + "|" + 
						m_pOrbiter->m_mapTextString[TEXT_Create_Directory_CONST] + "|" + 
						m_pOrbiter->m_mapTextString[TEXT_CANCEL_CONST],
					StringUtils::ltos(m_pOrbiter->m_dwPK_Device) + " " + StringUtils::ltos(nAppServer) + " " +
					"1 " + StringUtils::ltos(COMMAND_Spawn_Application_CONST) + " " +
					StringUtils::ltos(COMMANDPARAMETER_Filename_CONST) + " \"mkdir\" " + 
					StringUtils::ltos(COMMANDPARAMETER_Name_CONST) + " \"create_folder\" " +
					StringUtils::ltos(COMMANDPARAMETER_Arguments_CONST) + " \"" + sParentFolder + sNewFolder + "\"|", 
					"Folder creation", "0");

				m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_4_CONST, sParentFolder + sNewFolder + "/");
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuFileSave_CONST)
		{
			if(
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePrivate_CONST ||
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePublic_CONST
			)
			{
				m_sSaveFile_FileName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);

				string sSubDir = m_pOrbiter->m_iPK_MediaType == MEDIATYPE_pluto_DVD_CONST ? "videos" : "audio";
				if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objPlayListSavePublic_CONST)
					m_sSaveFile_FullBasePath = m_sSaveFile_MountedFolder + "public/data/" + sSubDir + "/";
				else
				{
					m_sSaveFile_FullBasePath = m_sSaveFile_MountedFolder + "user_" + StringUtils::itos(m_pOrbiter->m_dwPK_Users) + "/data/" + sSubDir + "/";

					//also append the user
					m_sSaveFile_Command += " " + StringUtils::ltos(COMMANDPARAMETER_Name_CONST) + 
						" " + "<%=" + StringUtils::ltos(VARIABLE_Seek_Value_CONST) + "%>";
				} 

				if(m_bSaveFile_Advanced_options)
					SaveFile_GotoChooseFolderDesignObj();
				else
					SaveFile_SendCommand();

				return true;
			}
			else if(pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butChooseDrive_CONST ||
				pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objCurrentUser_CONST)
			{
                m_sSaveFile_FileName = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);				
			}
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::FileSave_GridSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = dynamic_cast<DatagridCellBackData *>(pData);

	if(NULL != pCellInfoData && NULL != pCellInfoData->m_pDataGridCell)
	{
		if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseDrive_CONST)
		{
			string sValue = pCellInfoData->m_pDataGridCell->GetValue();
			vector<string> vectStrings;
			StringUtils::Tokenize(sValue, "\t", vectStrings);

			if(vectStrings.size() >= 2)
			{	
				m_nSaveFile_PK_DeviceDrive = atoi(vectStrings[0].c_str());
				m_sSaveFile_MountedFolder = vectStrings[1];

				m_sSaveFile_Drive = pCellInfoData->m_pDataGridCell->GetText();
				m_sSaveFile_RelativeFolder = "";

				m_pOrbiter->CMD_Set_Variable(VARIABLE_Device_List_CONST, StringUtils::ltos(m_nSaveFile_PK_DeviceDrive));
			}
		}
		else if(GetCurrentScreen_PK_DesignObj() == DESIGNOBJ_mnuChooseFolder_CONST)
		{
			m_sSaveFile_RelativeFolder = FileUtils::IncludeTrailingSlash(pCellInfoData->m_pDataGridCell->GetValue());

			if(m_sSaveFile_FullBasePath.length() < m_sSaveFile_RelativeFolder.length())
				m_sSaveFile_RelativeFolder = m_sSaveFile_RelativeFolder.substr(m_sSaveFile_FullBasePath.length());
			else
			{
				m_sSaveFile_RelativeFolder = FileUtils::BasePath(m_sSaveFile_RelativeFolder);

				if(m_sSaveFile_RelativeFolder.length() > 0)
				{
					if(m_sSaveFile_RelativeFolder[m_sSaveFile_RelativeFolder.length() - 1] != '/')
						m_sSaveFile_RelativeFolder += "/";
				}
			}

			m_pOrbiter->CMD_Set_Text(StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), 
				"Folder : " + m_sSaveFile_FullBasePath + m_sSaveFile_RelativeFolder, TEXT_STATUS_CONST);

			m_pOrbiter->Renderer()->RenderObjectAsync(m_pOrbiter->FindObject(DESIGNOBJ_mnuChooseFolder_CONST));
		}
	}

	return false;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SaveFile_GotoChooseFolderDesignObj()
{
	string sNewPath = FileUtils::IncludeTrailingSlash(m_sSaveFile_FullBasePath) + m_sSaveFile_RelativeFolder;

	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_5_CONST, FileUtils::ExcludeTrailingSlash(sNewPath) + "\nMT-1\nP");
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), "", "", 
		false, true);

	m_pOrbiter->CMD_Set_Text(StringUtils::ltos(DESIGNOBJ_mnuChooseFolder_CONST), 
		"Folder : " + sNewPath, TEXT_STATUS_CONST);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SaveFile_SendCommand()
{
	string sFilename = m_pOrbiter->m_mapVariable_Find(VARIABLE_Seek_Value_CONST);
	sFilename = FileUtils::ValidFileName(sFilename,true);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST,sFilename);

	string sPath = FileUtils::IncludeTrailingSlash(m_pOrbiter->m_mapVariable_Find(VARIABLE_Path_CONST));
	sPath = FileUtils::ValidFileName(sPath,true);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Path_CONST, sPath);

	m_pOrbiter->CMD_Send_Message(m_sSaveFile_Command, false);
	m_pOrbiter->GotoMainMenu();

	//reset file save info
	m_sSaveFile_MountedFolder = "";
	m_sSaveFile_RelativeFolder = "";
	m_sSaveFile_Drive = "";
	m_sSaveFile_FullBasePath = "";
	m_sSaveFile_FileName = "";
	m_sSaveFile_Command = "";
	m_bSaveFile_CreatingFolder = false;
	m_nSaveFile_PK_DeviceDrive = 0;
	m_bSaveFile_Advanced_options = true;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_Get_Capture_Card_Port(long PK_Screen, int iPK_Device, string sName, string sDescription, string ssComments)
{
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_Device));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sName);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, ssComments);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_Device_1_CONST, StringUtils::ltos(iPK_Device));
	ScreenHandlerBase::SCREEN_Get_Capture_Card_Port(PK_Screen, iPK_Device, sName, sDescription, ssComments);
}

