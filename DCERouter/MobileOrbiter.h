#ifndef MOBILEORBITER_H
#define MOBILEORBITER_H

#include "../pluto_main/Define_DesignObj.h"

#define MENUTYPE_BACK			-1
#define MENUTYPE_REMOTE			-2
#define MENUTYPE_REFRESH		-3

#define MENUTYPE_MAINMENU		0
#define MENUTYPE_LIGHTING		1
#define MENUTYPE_MEDIA			2
#define MENUTYPE_CLIMATE		3
#define MENUTYPE_TELECOM		4
#define MENUTYPE_SECURITY		5
#define MENUTYPE_OTHER			6
#define MENUTYPE_OTHER_MEDIA	7

// The browsing & guides
#define MENUTYPE_MEDIA_FILES	DESIGNOBJ_MNUMEDIAFILELIST_CONST

// The remotes
#define MENUTYPE_REMOTE_PLUTO_TV		DESIGNOBJ_MNUPVR_CONST
#define MENUTYPE_REMOTE_PLUTO_EPG		DESIGNOBJ_MNUTVEPG1_CONST
#define MENUTYPE_REMOTE_NON_PLUTO_TV	DESIGNOBJ_MNUSATELLITECABLEBOX_CONST
#define MENUTYPE_REMOTE_TUNERS			22
#define MENUTYPE_REMOTE_NPTV_MORE		23
#define MENUTYPE_REMOTE_PLAYLIST_ONE	DESIGNOBJ_MNUPLAYPLAYLIST_CONST
#define MENUTYPE_REMOTE_PLAYLIST_LIST	24
#define MENUTYPE_REMOTE_DVD				DESIGNOBJ_MNUDVDREMOTE_CONST
#define MENUTYPE_REMOTE_DVDMENU			DESIGNOBJ_MNUDVDMENU_CONST
#define MENUTYPE_REMOTE_MEDIA_ADVANCED	DESIGNOBJ_MNUADVANCEDMEDIAOPTIONS_CONST
#define MENUTYPE_RESEND_IR				25
#define MENUTYPE_REMOTE_COMPUTING		DESIGNOBJ_MNUPCOPTIONS_CONST
#define MENUTYPE_REMOTE_WEB				DESIGNOBJ_MNUBROWSERCONSOLE_CONST


// Other screens
#define MENUTYPE_ALL_CAMERAS			DESIGNOBJ_MNUSECURITYCAMERAS_CONST
#define MENUTYPE_VIEW_CAMERA			DESIGNOBJ_MNUSINGLECAMERAVIEWONLY_CONST
#define MENUTYPE_LOCATIONS				DESIGNOBJ_MNUCURRENTLOCATION_CONST

#define MENUTYPE_HELP1					30
#define MENUTYPE_HELP2					31
#define MENUTYPE_HELP3					32

// Temporary screens for Joe's demo
#define MENUTYPE_JOESINVOICE					990
#define MENUTYPE_JOESPMTMETHOD					991
#define MENUTYPE_JOESPINCODE					992
#define MENUTYPE_JOESPAYMENTOK					993
#define MENUTYPE_JOESBADPINCODE					994



class MobileOrbiter : public AlarmEvent
{
public:
	class DeviceData_Router *m_pDevice_CurrentDetected;
	class DeviceData_Router *m_pDevice_This;
	class Router *m_pRouter;
	class EntGroup *m_pLastEntgroup;
	class DeviceData_Router *m_pEntGroupAudioZone_LockedOn;
	class DCERoom *m_pRoom_LockedOn;
	class PlutoOrbiter *m_pPlutoOrbiter;
	class DCEMI_PS_MobileOrbiter *m_pDCEMI_PS_MobileOrbiter;
	
	int m_iLastMenuType;
	string m_sID,m_sWidth,m_sHeight,m_sCurrentInput;
	bool m_bBuildingMenu,m_bMaskInput;
	bool m_bFollowMeLights,m_bFollowMeClimate,m_bFollowMeMedia,m_bFollowMeTelecom,m_bFollowMeSecurity;
	int m_iLastSignalStrength,m_EnterKeyAlarm,m_iLastPage;

	string m_sLastBasketID,m_sLastAmount,m_sLastPin; // Temporary Joe's
	pluto_pthread_mutex_t m_MoMutex;

	MobileOrbiter(class DCEMI_PS_MobileOrbiter *pDCEMI_PS_MobileOrbiter,class PlutoOrbiter *pPlutoOrbiter,string ID,class DeviceData_Router *pDevice);

	void ShowMenu(int MenuType,Message *pOriginatingMessage) {};

	// 0 main menu
	void CreateMainMenu(string &sText,string &sTokens);
	void CreateLocationsMenu(string &sText,string &sTokens,string &sMenuList);

	// 1, 3-7 array menus
	void CreateArrayMenu(string &sText,string &sTokens,int PK_C_Array);
	void CreateSecurityMenu(string &sText,string &sTokens);

	// 2 media menu
	void CreatePlutoMediaMenu(string &sText,string &sTokens);

	// 12 media files
	void CreateMediaFilesMenu(string &sText,string &sTokens,string &sMenuList,Message *pMessage);

	// 20 Pluto TV
	void CreatePlutoTVRemote(string &sText,string &sTokens);
	// 21 Pluto EPG
	void CreateEPGRemote(string &sText,string &sTokens,string &sMenuList);
	// 23 Non-pluto TV
	void CreateNonPlutoTVRemote(string &sText,string &sTokens);
	// Non-pluto TV more
	void CreateNonPlutoTVRemoteMore(string &sText,string &sTokens);
	void CreateNonPlutoTVTuners(string &sText,string &sTokens,string &sMenuList);

	// 23 Playlist one
	void CreatePlaylistOneRemote(string &sText,string &sTokens);
	// 24 Playlist List
	void CreatePlaylistListRemote(string &sText,string &sTokens,string &sMenuList);
	// 25 DVD
	void CreateDVDRemote(string &sText,string &sTokens);
	void CreateDVDRemoteMenu(string &sText,string &sTokens);

	// Computing
	void CreateRemoteComputing(string &sText,string &sTokens);
	void CreateRemoteWeb(string &sText,string &sTokens,string &sMenuList);

	void CreateRemoteMediaAdvancedOptions(string &sText,string &sTokens,string &sMenuList);
	void CreateRemoteMediaResendIR(string &sText,string &sTokens,string &sMenuList);

	void CreateCameraViewMenu(string &sText,string &sTokens,Message *pMessage);


	// Temporary - Joe's hacked in demo
	void CreateJoesInvoiceMenu(string &sText,string &sTokens,Message *pMessage);
	void CreateJoesPinMenu(string &sText,string &sTokens,Message *pMessage);

	void AddNowPlayingInfo(string &sTokens,bool bWithPicture,int Lines=20);
	void Keypress(string Key) {};
	char MapShortCutCounterToKey(int ShortCutCounter);
	void AlarmCallback(int id, void* param);
	void NewAssociation(DeviceData_Router *pDevice_PriorDetected) {};
	void RemovingAssocation() {};
};

#endif
