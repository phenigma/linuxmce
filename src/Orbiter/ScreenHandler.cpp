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
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
using namespace DCE;

//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : 
	ScreenHandlerBase(p_MapDesignObj)
	, m_MapMutex("maps")
	, mediaFileBrowserOptions(pOrbiter)
{
	m_pOrbiter = pOrbiter;

	m_MapMutex.Init(NULL);
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
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(StringUtils::itos(iPK_PnpQueue));
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), StringUtils::itos(iPK_PnpQueue), "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sName)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::ltos(iPK_Users));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Seek_Value_CONST, sName);
	ScreenHandlerBase::SCREEN_CDTrackCopy(PK_Screen, iPK_Users, sName);
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
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaBrowser_ObjectSelected,	new ObjectInfoBackData());
	return;
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaBrowser_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if(	pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_dgFileList2_CONST	|| pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_dgFileList2_Pics_CONST || pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_objCDCover_CONST ) // todo
	{
		DesignObj_Orbiter *pObj_Play = m_pOrbiter->FindObject( TOSTRING(DESIGNOBJ_popFileDetails_CONST) ".0.0." TOSTRING(5089) );
		if( !pObj_Play || !pObj_Play->m_pParentObject )
			return false; // Shouldn't happen

		DataGridCell *pCell_Pic=NULL,*pCell_List=NULL;
		if( mediaFileBrowserOptions.m_pObj_PicGrid->m_pDataGridTable )
			pCell_Pic = mediaFileBrowserOptions.m_pObj_PicGrid->m_pDataGridTable->GetData(mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedColumn,mediaFileBrowserOptions.m_pObj_PicGrid->m_iHighlightedRow);
		if( mediaFileBrowserOptions.m_pObj_ListGrid->m_pDataGridTable )
			pCell_List = mediaFileBrowserOptions.m_pObj_ListGrid->m_pDataGridTable->GetData(0,mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow);

		if( !pCell_List )
			return false; // Shouldn't happen

		if( !pCell_List->m_Value || (pCell_List->m_Value[0]=='!' && pCell_List->m_Value[1]=='D') )
		{
			// It's a sub directory.  Update the source and refresh the page
			mediaFileBrowserOptions.m_sSources = pCell_List->m_Value ? pCell_List->m_Value : "";
			m_pOrbiter->CMD_Refresh("*");
			return false;
		}


		GetAttributesForMediaFile(pCell_List->m_Value);

		size_t Size=0;
		char *pData = NULL;
		if( pCell_Pic && pCell_Pic->m_ValueLength )
		{
			pData = FileUtils::ReadFileIntoBuffer(pCell_Pic->m_Value,Size);
			if( !pData )
			{
				int i=0;
				DCE::CMD_Request_File CMD_Request_File( m_pOrbiter->m_dwPK_Device, m_pOrbiter->m_dwPK_Device_GeneralInfoPlugIn, 
					pCell_Pic->m_Value, &pData, &i );
				m_pOrbiter->SendCommand( CMD_Request_File );
				Size=i;
			}
		}

		m_pOrbiter->CMD_Update_Object_Image(pObj_Play->m_pParentObject->m_ObjectID + "." TOSTRING(DESIGNOBJ_objCDCover_CONST),"jpg",pData,int(Size),"0");
		m_pOrbiter->m_pObj_Highlighted = pObj_Play;
		m_pOrbiter->CMD_Show_Popup(pObj_Play->m_pParentObject->m_ObjectID,10,10,"","filedetails",false,false);
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse(pObj_Play->m_pParentObject->m_rPosition + pObj_Play->m_pPopupPoint );
		m_pOrbiter->m_pMouseBehavior->SetMousePosition(pObj_Play);
#endif
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_Close_CONST )
	{
		m_pOrbiter->m_pObj_Highlighted = mediaFileBrowserOptions.m_pObj_ListGrid;
		m_pOrbiter->CMD_Remove_Popup("","filedetails");
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
	}
	else if( pObjectInfoData->m_PK_DesignObj_SelectedObject == 5089 )
	{
		if( !mediaFileBrowserOptions.m_pObj_ListGrid->m_pDataGridTable )
			return false; //shouldn't happen

		DataGridCell *pCell_List=NULL;
		pCell_List = mediaFileBrowserOptions.m_pObj_ListGrid->m_pDataGridTable->GetData(0,mediaFileBrowserOptions.m_pObj_ListGrid->m_iHighlightedRow);

		if( !pCell_List || !pCell_List->m_Value )
			return false; // Shouldn't happen

		m_pOrbiter->CMD_Remove_Popup("","filedetails");
#ifdef ENABLE_MOUSE_BEHAVIOR
		m_pOrbiter->m_pMouseBehavior->ConstrainMouse();
#endif
		DCE::CMD_MH_Play_Media CMD_MH_Play_Media(m_pOrbiter->m_dwPK_Device,m_pOrbiter->m_dwPK_Device_MediaPlugIn,
			0,pCell_List->m_Value,0,0,StringUtils::itos( m_pOrbiter->m_pLocationInfo->PK_EntertainArea ),false,0);
		m_pOrbiter->SendCommand(CMD_MH_Play_Media);
	}

	return false;
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
void ScreenHandler::SCREEN_MediaSortFilter(long PK_Screen)
{
	// Preseed the selected values
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaType_CONST)), mediaFileBrowserOptions.m_PK_MediaType );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_MediaSubType_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0"), mediaFileBrowserOptions.m_sPK_MediaSubType );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PK_FileFormat_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0"), mediaFileBrowserOptions.m_sPK_FileFormat );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Genres_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0"), mediaFileBrowserOptions.m_sPK_Attribute_Genres );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_Sort_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0"), mediaFileBrowserOptions.m_PK_AttributeType_Sort );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_MediaSource_CONST) ".<%=" TOSTRING(VARIABLE_PK_MediaType_CONST) "%>.0"), mediaFileBrowserOptions.m_sSources );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_PrivateMedia_CONST)), mediaFileBrowserOptions.m_sPK_Users_Private );
	mediaFileBrowserOptions.SelectArrays( m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFBSF_RatingsByUser_CONST)), mediaFileBrowserOptions.m_PK_Users );

	ScreenHandlerBase::SCREEN_MediaSortFilter(PK_Screen);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::MediaSortFilter_ObjectSelected,	new ObjectInfoBackData());
}
//-----------------------------------------------------------------------------------------------------
bool ScreenHandler::MediaSortFilter_ObjectSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	if( pObjectInfoData->m_pObj && pObjectInfoData->m_pObj->m_pParentObject )
	{
		m_pOrbiter->Renderer()->RenderObjectAsync(pObjectInfoData->m_pObj);  // We will be changing the selected state
		switch( pObjectInfoData->m_pObj->m_pParentObject->m_iBaseObjectID )
		{
		case DESIGNOBJ_popFBSF_Genres_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_Attribute_Genres,true);
			return true;
		case DESIGNOBJ_popFBSF_PK_FileFormat_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_FileFormat,true);
			return true;
		case DESIGNOBJ_popFBSF_MediaType_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_MediaType);
			return true;
		case DESIGNOBJ_popFBSF_PrivateMedia_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_Users_Private,false);
			return true;
		case DESIGNOBJ_popFBSF_RatingsByUser_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_Users);
			return true;
		case DESIGNOBJ_popFBSF_Sort_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_PK_AttributeType_Sort);
			return true;
		case DESIGNOBJ_popFBSF_MediaSource_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sSources,true);
			return true;
		case DESIGNOBJ_popFBSF_PK_MediaSubType_CONST:
			mediaFileBrowserOptions.SelectedArray(pObjectInfoData->m_pObj,mediaFileBrowserOptions.m_sPK_MediaSubType,true);
			return true;
		};
	}
	if(	pObjectInfoData->m_PK_DesignObj_SelectedObject == DESIGNOBJ_butFBSF_OK_CONST )
	{
		m_pOrbiter->CMD_Goto_Screen("",mediaFileBrowserOptions.m_iPK_Screen);
		return true;
	}
	return false;
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
			pDesignObj_Orbiter->m_GraphicToDisplay = sValues.size()==0 ? GRAPHIC_SELECTED : GRAPHIC_NORMAL;
		else if( ("," + sValues + ",").find( "," + sArrayValue + "," )!=string::npos )
			pDesignObj_Orbiter->m_GraphicToDisplay = GRAPHIC_SELECTED;
		else
			pDesignObj_Orbiter->m_GraphicToDisplay = GRAPHIC_NORMAL;
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
			pDesignObj_Orbiter->m_GraphicToDisplay = GRAPHIC_SELECTED;
		else
			pDesignObj_Orbiter->m_GraphicToDisplay = GRAPHIC_NORMAL;
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
				pObj->m_GraphicToDisplay = GRAPHIC_NORMAL;
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
				pObj->m_GraphicToDisplay = GRAPHIC_SELECTED;
				for( DesignObj_DataList::iterator iHao=pObj->m_pParentObject->m_ChildObjects.begin(  ); iHao != pObj->m_pParentObject->m_ChildObjects.end(  ); ++iHao )
				{
					DesignObj_Orbiter *pObj_Child=( DesignObj_Orbiter * )*iHao;
					if( pObj_Child!=pObj )
					{
						pObj_Child->m_GraphicToDisplay = GRAPHIC_NORMAL;
						m_pOrbiter->Renderer()->RenderObjectAsync(pObj_Child);
					}
				}
				sValues="";
				return;
			}
			else  // User selected something besides 'all'
			{
				pObj_All->m_GraphicToDisplay = GRAPHIC_NORMAL;
				m_pOrbiter->Renderer()->RenderObjectAsync(pObj_All);
			}
		}
	}

	pObj->m_GraphicToDisplay = GRAPHIC_SELECTED;
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
		pObj_PriorSelected->m_GraphicToDisplay = GRAPHIC_NORMAL;
		m_pOrbiter->Renderer()->RenderObjectAsync(pObj_PriorSelected);  // We will be changing the selected state
	}

	pObj->m_GraphicToDisplay = GRAPHIC_SELECTED;
	iValue = iValue_New;
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
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(m_p_MapDesignObj_Find(PK_Screen)), sMacAddress, "", false, false );
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_WhatModelMobileOrbiter(long PK_Screen, string sMacAddress)
{
	m_pOrbiter->m_pScreenHistory_NewEntry->ScreenID(sMacAddress);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMacAddress);
	ScreenHandlerBase::SCREEN_WhatModelMobileOrbiter(PK_Screen, sMacAddress);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_NewPlugAndPlayDevice(long PK_Screen, int iPK_Device,
												   string sDescription, string sComments)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sDescription);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sComments);
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_3_CONST, StringUtils::ltos(iPK_Device));
	GotoScreen(SCREEN_NewPlugAndPlayDevice_CONST,StringUtils::ltos(iPK_Device),true,true);
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
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::Computing_ObjectSelected(CallBackData *pData)
{
	DatagridCellBackData *pCellInfoData = (DatagridCellBackData *)pData;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::Computing_DatagridSelected(CallBackData *pData)
{
	ObjectInfoBackData *pObjectInfoData = (ObjectInfoBackData *)pData;

	switch(GetCurrentScreen_PK_DesignObj())
	{
		case DESIGNOBJ_YourName_CONST:
			break;
	};
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
	string sOption3, string sMessage3, string sOption4, string sMessage4)
{
	int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
	if( !PK_DesignObj )
		PK_DesignObj = PK_Screen;  // Allow the user to pass in a design obj to use instead
	string sPK_DesignObj = StringUtils::ltos(PK_DesignObj);

	//if(sMessage == "")
	//	sMessage = "Unable to save playlist";

	m_pOrbiter->CMD_Set_Timeout(sPK_DesignObj, sTimeout);
//	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, "", "", false, false);

	m_pOrbiter->CMD_Goto_DesignObj(0, sPK_DesignObj, "", "", false, bCantGoBack );
	m_pOrbiter->CMD_Set_Text(sPK_DesignObj, sMessage, TEXT_STATUS_CONST);

	if(bPromptToResetRouter)
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butRestartDCERouter_CONST), 0, "", "", "1" );

	if( sOption1.size()==0 && sOption2.size()==0 && sOption3.size()==0 && sOption4.size()==0 )
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), m_pOrbiter->m_mapTextString[TEXT_Ok_CONST], TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, "0 -300 1 " TOSTRING(COMMAND_Go_back_CONST) " " TOSTRING(COMMANDPARAMETER_Force_CONST) " 1");
		return;
	}

	if(sOption1.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse1_CONST), sOption1, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_1_CONST, sMessage1);
	}
	if(sOption2.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse2_CONST), sOption2, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_2_CONST, sMessage2);
	}
	if(sOption3.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse3_CONST), sOption3, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_3_CONST, sMessage3);
	}
	if(sOption4.size())
	{
		m_pOrbiter->CMD_Show_Object(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), 0, "", "", "1" );
		m_pOrbiter->CMD_Set_Text(sPK_DesignObj + ".0.0." + StringUtils::itos(DESIGNOBJ_butResponse4_CONST), sOption4, TEXT_STATUS_CONST);
		m_pOrbiter->CMD_Set_Variable(VARIABLE_Message_4_CONST, sMessage4);
	}
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
void ScreenHandler::SCREEN_New_Phone_Enter_Number(long PK_Screen, int iPK_Device, string sPhoneName)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, StringUtils::ltos(iPK_Device));
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_2_CONST, sPhoneName);

	GotoScreen(SCREEN_New_Phone_Enter_Number_CONST,StringUtils::ltos(iPK_Device),true,true);
	RegisterCallBack(cbObjectSelected, (ScreenHandlerCallBack) &ScreenHandler::New_Phone_Enter_Number_DeviceConfigured,
		new ObjectInfoBackData());
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
		string sName = m_pOrbiter->m_mapVariable[VARIABLE_Misc_Data_2_CONST];
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
void ScreenHandler::SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sPK_Device)
{
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sPK_Device);
	GotoScreen(SCREEN_Sensors_Viewed_By_Camera_CONST,sPK_Device,true,true);
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
