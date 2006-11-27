#ifndef __SCREEN_HANDLER_H__
#define __SCREEN_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "Gen_Devices/AllScreens.h"
#include "Orbiter.h"
#include "CallBackData.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"

//-----------------------------------------------------------------------------------------------------
typedef bool (ScreenHandler::*ScreenHandlerCallBack)(CallBackData *pData);
//-----------------------------------------------------------------------------------------------------
typedef class ScreenHandler * (* RAOP_FType) (class Orbiter *,  map<int,int> *p_MapDesignObj, Logger *);
//-----------------------------------------------------------------------------------------------------

class MediaFileBrowserOptions
{
public:
	int m_PK_MediaType,m_PK_AttributeType_Sort,m_PK_Users,m_iPK_Screen;
	string m_sPK_MediaSubType,m_sPK_FileFormat,m_sPK_Attribute_Genres,m_sSources,m_sPK_Users_Private;
	string m_sSelectedFile;  // Used to hold the selected value
	list<int> m_listPK_AttributeType_Sort_Prior;
	list< pair<int,string> >m_listPK_Attribute_Description;  // For drilling down on attributes
	map< pair<int,string>, DesignObj_Orbiter * > m_mapObjectsValues;
	Orbiter *m_pOrbiter;
	DesignObj_DataGrid *m_pObj_ListGrid,*m_pObj_PicGrid;

	MediaFileBrowserOptions(Orbiter *pOrbiter) 
	{ 
		m_pOrbiter=pOrbiter; 
		ClearAll(0,0,0); 
		m_pObj_ListGrid=m_pObj_PicGrid=NULL;
	}

	string ToString()
	{
		string sResult = StringUtils::itos(m_PK_MediaType) + "|" + m_sPK_MediaSubType + "|" + m_sPK_FileFormat + "|" + m_sPK_Attribute_Genres + "|" + m_sSources +
			"|" + m_sPK_Users_Private + "|" + StringUtils::itos(m_PK_AttributeType_Sort) + "|" + StringUtils::itos(m_PK_Users) + " | ";
		if( m_listPK_Attribute_Description.size() )
			sResult += StringUtils::itos(m_listPK_Attribute_Description.front().first);
		return sResult;
	}

	string HumanReadable();

	void ClearAll(int PK_MediaType,int PK_Screen,int PK_AttributeType_Sort)
	{
		m_iPK_Screen=PK_Screen;
		m_PK_MediaType=PK_MediaType;
		m_PK_Users=0;
		m_PK_AttributeType_Sort = PK_AttributeType_Sort;
		m_listPK_AttributeType_Sort_Prior.clear();
		m_listPK_Attribute_Description.clear();
		m_sPK_MediaSubType=""; m_sPK_FileFormat=""; m_sPK_Attribute_Genres=""; m_sSources=""; m_sPK_Users_Private="0";
		m_pOrbiter->CMD_Set_Variable(VARIABLE_PK_MediaType_CONST, StringUtils::itos(m_PK_MediaType)); 
		m_mapObjectsValues.clear();
		if( !m_pObj_ListGrid )
			m_pObj_ListGrid=(DesignObj_DataGrid *) m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgFileList2_CONST));
		if( !m_pObj_PicGrid )
			m_pObj_PicGrid=(DesignObj_DataGrid *) m_pOrbiter->FindObject(TOSTRING(DESIGNOBJ_popFileList_CONST) ".0.0." TOSTRING(DESIGNOBJ_dgFileList2_Pics_CONST));
	}

	void SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,string &sValues);
	void SelectArrays(DesignObj_Orbiter *pObj,int PK_Array,int &iValue);
	void SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,string &sValues,bool bTreatZeroAsAll);
	void SelectedArray(DesignObj_Orbiter *pObj,int PK_Array,int &iValue);
};

class ScreenHandler : public ScreenHandlerBase
{
protected:
	Orbiter *m_pOrbiter;
	map<CallBackType, ScreenHandlerCallBack> m_mapCallBack;
	map<CallBackType, CallBackData *> m_mapCallBackData;
	string m_sActiveApplication_Description,m_sActiveApplication_Window;
	int m_PK_Screen_ActiveApp_OSD,m_PK_Screen_ActiveApp_Remote;
	int m_iStage; // Some screens go through stages and need a variable to track that
	time_t m_tLastDeviceAdded;

	pluto_pthread_mutex_t m_MapMutex; /** < Protected the access to our maps */

	virtual void DisplayMessageOnOrbiter(int PK_Screen,
		string sMessage, bool bPromptToResetRouter = false,
		string sTimeout = "0", bool bCantGoBack = false, 
		string sOption1 = "", string sMessage1 = "",
		string sOption2 = "", string sMessage2 = "", 
		string sOption3 = "", string sMessage3 = "",
		string sOption4 = "", string sMessage4 = "",
		string sID="");


public: 
	ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);
	virtual ~ScreenHandler();

	//used to register as plugin
	virtual bool Register() { return 0; }

	//callback related stuff
	void RegisterCallBack(CallBackType aCallBackType, ScreenHandlerCallBack aScreenHandlerCallBack, 
		CallBackData *pCallBackData);
	virtual void ResetCallBacks();
	ScreenHandlerCallBack m_mapCallBack_Find(CallBackType aCallBackType);
	CallBackData *m_mapCallBackData_Find(CallBackType aCallBackType);

	map<string,string> m_mapKeywords;  // Used for storing keywords to be substituded into text

	//helper functions
	int GetCurrentScreen_PK_DesignObj();
	void RefreshDatagrid(long PK_DesignObj_Datagrid);
	void BadGotoScreen(int PK_Screen);

	//screens functions
	virtual void GotoDesignObj(int PK_DesignObj,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false);
	virtual void SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue);
	virtual void SCREEN_New_Pnp_Device_One_Possibility(long PK_Screen, int iPK_Room, int iPK_DHCPDevice, string sDescription, string ssComments, int iPK_PnpQueue);
	bool Pnp_ObjectSelected(CallBackData *pData);
	virtual void SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users, string sName, int iDriveID); 
	MediaFileBrowserOptions mediaFileBrowserOptions;  // Current sort/filter options for the media browser

	virtual void SCREEN_FileList_Music_Movies_Video(long PK_Screen);
	bool MediaBrowser_ObjectSelected(CallBackData *pData);
	string GetFileBrowserPopup(DesignObj_Orbiter *pObj_MenuPad);
	bool MediaBrowser_DatagridSelected(CallBackData *pData);
	void SelectedAttributeCell(DataGridCell *pCell);
	bool MediaBrowser_Render(CallBackData *pData);
	void SetMediaSortFilterSelectedObjects();
	void GetAttributesForMediaFile(const char *pFilename);
	virtual void SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription, int iPK_PnpQueue);
	virtual void SCREEN_WhatModelMobileOrbiter(long PK_Screen, int iPK_Users, string sMac_address);
	virtual void SCREEN_Pick_Room_For_Device(long PK_Screen, int iPK_Device, string sDescription, string ssComments);
	virtual void SCREEN_SensorsNotReady(long PK_Screen, string sDescription);
	virtual void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, 
		string sExitDelay, string sAlerts);
	virtual void SCREEN_CurrentlyActiveRemote(long PK_Screen);
	virtual void SCREEN_PopupMessage(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack);
	virtual void SCREEN_GenericKeyboard(long PK_Screen, string sText, string sCommand_Line, string sDescription, string sCannotGoBack){ SCREEN_PopupMessage(PK_Screen, sText, sCommand_Line, sDescription, "0", "0", sCannotGoBack); }  // Treat this like the popup message

	virtual void SCREEN_GenericAppController(long PK_Screen);
	virtual void SCREEN_GenericAppFullScreen(long PK_Screen);
	virtual void SCREEN_Computing(long PK_Screen);
	bool Computing_ObjectSelected(CallBackData *pData);
	bool Computing_DatagridSelected(CallBackData *pData);

	//dialogs
	virtual void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors);
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
	virtual void SCREEN_DialogAskToResume(long PK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, 
		string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String, int iPK_Screen_GoTo);
	virtual void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack);
	virtual void SCREEN_DialogPhoneInstructions(long PK_Screen, string sInstructions, string sPhoneName);
	virtual void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMacAddress, string sCommandLine, 
		string sPK_DeviceFrom, string sPhoneName, string sPK_Device_AppServer);
	virtual void SCREEN_Floorplan(long PK_Screen, string sPK_DesignObj);

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

	virtual void SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue);
	virtual void SCREEN_Get_Username_Password_For_Devices(long PK_Screen, bool bAlready_processed, string sDescription, int iPK_PnpQueue);

	virtual void SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device, string sPK_Device_AppServer);

	virtual void SCREEN_CreateViewBookmarks(long PK_Screen);
	bool Bookmark_GridSelected(CallBackData *pData);

	//cameras
	virtual void SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device);
	virtual void SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device);
	virtual void SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, string sOptions, int iPK_PnpQueue);
	bool Sensors_ObjectSelected(CallBackData *pData);

	virtual bool New_Phone_Enter_Number_DeviceConfigured(CallBackData *pData);
	virtual void SCREEN_Add_Software(long PK_Screen);
	bool AddSoftware_ObjectHighlighted(CallBackData *pData);
	bool AddSoftware_GridRendering(CallBackData *pData);
	bool AddSoftware_GridSelected(CallBackData *pData);

	virtual void SCREEN_FileSave(long PK_Screen, string sCaption, string sCommand, bool bAdvanced_options);
	bool FileSave_ObjectSelected(CallBackData *pData);
	bool FileSave_GridSelected(CallBackData *pData);

	void SCREEN_Get_Capture_Card_Port(long PK_Screen, int iPK_Device, string sName, string sDescription, string ssComments);

	//helper methods
	void SaveFile_GotoChooseFolderDesignObj();
	void SaveFile_SendCommand();

	// Choose provider
	virtual void SCREEN_Choose_Provider_for_Device(long PK_Screen, int iPK_Device, string sText, string sDescription);
	bool ChooseProvider_Intercepted(CallBackData *pData);
	bool ChooseProvider_ObjectSelected(CallBackData *pData);
	bool ChooseProvider_DatagridSelected(CallBackData *pData);
	void ChooseProviderGetNextStage();
	void SpawnProviderScript(string sCommandLine,string sArguments);

	//savefile data
	string m_sSaveFile_MountedFolder;
	string m_sSaveFile_RelativeFolder;
	string m_sSaveFile_Drive;
	string m_sSaveFile_FullBasePath;
	string m_sSaveFile_FileName;
	string m_sSaveFile_Command;
	bool m_bSaveFile_CreatingFolder;
	bool m_bSaveFile_Advanced_options;
	int m_nSaveFile_PK_DeviceDrive;
};
//-----------------------------------------------------------------------------------------------------
#endif
