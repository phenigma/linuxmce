#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "VIPShared/MOMenuItem.h"
#include "../pluto_main/Table_Command.h"
#include "../pluto_main/Table_CommandParameter.h"
#include "../pluto_main/Table_C_Array.h"
#include "../pluto_main/Table_DeviceTemplate.h"
#include "../pluto_main/Table_DeviceCategory.h"
#include "MobileOrbiter.h"
#include "PlutoOrbiter.h"
#include "Router.h"
#include "DCEMI_PS_VirtualDevices.h"

// Available keys are 0123456789YNCEUDLRPMAB 
// (yes/dial, no/hangup, clear, enter, up, down, left, right, pencil, menu, A=soft button left, B=soft button right)
// An ! before the letter means when the button was held down

const char *GlobalOptions = "*N~off~~{c} {e} 1 342~1001~help~~{c} {c} 1 22 6 30~";
const char *SkipUpDownBackFwdNav = "*L~left~~{c} {e} 1 68~*R~right~~{c} {e} 1 63~"\
	"U~up~~{c} {e} 1 7~D~down~~{c} {e} 1 6~"\
	"*E~enter~~{c} {e} 1 19~";
const char *UDLREnterNav = "*L~left~~{c} {e} 1 39~*R~right~~{c} {e} 1 40~"\
	"*U~up~~{c} {e} 1 37~*D~down~~{c} {e} 1 38~"\
	"*E~enter~~{c} {e} 1 41~";
const char *SecurityMenu = 
	"*1~1~~{c} 0 1 232 8 1~*2~2~~{c} 0 1 232 8 3~"\
	"*3~3~~{c} 0 1 232 8 5~*4~4~~{c} 0 1 232 8 2~"\
	"*5~5~~{c} 0 1 232 8 6~*6~6~~{c} 0 1 232 8 4~"\
	"*7~7~~{c} 0 1 232 8 105~";
const char *MediaFilesMenu =
	// ID 1 means add the plus, 2 means add the *
	"A~queue~~{c} {mod} 1 209 53 1~"\
	"B~remote~~{c} {c} 1 22 6 -2~";

const char *StopStream = "N~stop~~{c} {e} 1 342~";
const char *PagableList = "*~ast~~{c} {mod} 1 Y~#~num~~{c} {mod} 1 198~"\
	"1~1~~{c} {mod} 1 209 67 0~2~2~~{c} {mod} 1 209 67 1~"\
	"3~3~~{c} {mod} 1 209 67 2~4~4~~{c} {mod} 1 209 67 3~"\
	"5~5~~{c} {mod} 1 209 67 4~6~6~~{c} {mod} 1 209 67 5~"\
	"7~7~~{c} {mod} 1 209 67 6~8~8~~{c} {mod} 1 209 67 7~"\
	"9~9~~{c} {mod} 1 209 67 8~0~0~~{c} {mod} 1 209 67 9~";
const char *MainMenuOptions = "C~~~{c} {c} 1 22 6 -2~"\
	"*Y~~~{c} {c} 1 413 53 2~"\
	"A~~~{c} {c} 1 22 6 2437~"\
	"!1~~~{c} -12 1 411 53 1~"\
	"!2~~~{c} -12 1 411 53 2~"\
	"!3~~~{c} -12 1 411 53 3~"\
	"!4~~~{c} -12 1 411 53 4~"\
	"!5~~~{c} -12 1 411 53 5~";
const char *Computing = "*1~office~~{c} -23 1 319~*2~~~{c} -12 1 381 5 {e} 53 0~*3~~~{c} -12 1 381 5 {e} 53 1~*N~stop~~{c} -23 1 321&{c} -23 1 322~";
const char *LightsUpDown = "*A~left~~{c} -12 1 413 53 -10~*B~right~~{c} -12 1 413 53 +10~";
const char *VolumeUpDown = "*A~left~~{c} {e} 1 9~*B~right~~{c} {e} 1 8~";
const char *MainMenu = "C~~~{c} {c} 1 22 6 0~";  // Go to main menu
const char *CameraMenu = "C~~~{c} {c} 1 22 6 5~";  // Go to main menu
const char *GotoAdvancedMedia = "Y~~~{c} {c} 1 22 6 2262~";
const char *GotoAdvancedMediaFromIR = "P~~~{c} {c} 1 22 6 2262~";
const char *GotoResendIR = "P~~~{c} {c} 1 22 6 25~";
const char *ReturnToRemote = "Y~~~{c} {c} 1 22 6 -2~";
const char *GotoOtherNPOptions = "P~~~{c} {c} 1 22 6 23~";
const char *NPTVOptions = "C~~~{c} {c} 1 22 6 23~";
const char *GoToNPTV = "P~~~{c} {c} 1 22 6 2427~";  // Go to non-pluto tv
const char *GoToPlutoTV = "P~~~{c} {c} 1 22 6 2121~";  // Go to pluto tv
const char *DVDRemote = "P~menu~~{c} {e} 1 51&{c} {e} 1 228 5 {c} 53 0 6 2165.0.0~";
const char *MoreNPTVOptions = "*1~1~~{c} {e} 1 43~*2~2~~{c} {e} 1 42~"\
	"*3~3~~{c} {e} 1 407~*4~4~~{c} {e} 1 57~"\
	"*5~5~~{c} {e} 1 408~*6~6~~{c} {e} 1 414~"\
	"*7~7~~{c} {e} 1 298~*8~8~~{c} {e} 1 177~"\
	"*9~9~~{c} {e} 1 19~0~0~~{c} {c} 1 22 6 22~"\
	"#~num~~{c} {c} 1 22 6 2427~";
const char *TypeAndSeek = "U~up~~{c} {mod} 1 37~D~down~~{c} {mod} 1 38~"\
	"1~1~~{c} {mod} 1 401 44 1~2~2~~{c} {mod} 1 401 44 2~"\
	"3~3~~{c} {mod} 1 401 44 3~4~4~~{c} {mod} 1 401 44 4~"\
	"5~5~~{c} {mod} 1 401 44 5~6~6~~{c} {mod} 1 401 44 6~"\
	"7~7~~{c} {mod} 1 401 44 7~8~8~~{c} {mod} 1 401 44 8~"\
	"9~9~~{c} {mod} 1 401 44 9~0~0~~{c} {mod} 1 401 44 0~"\
	"E~ENTER~~{c} {mod} 1 209 53 2~";
const char *SimulateKeyPresses = "1~1~~{c} {c} 1 173 53 1~2~2~~{c} {c} 1 173 53 2~3~3~~{c} {c} 1 173 53 3~"\
	"4~4~~{c} {c} 1 173 53 4~5~5~~{c} {c} 1 173 53 5~6~6~~{c} {c} 1 173 53 6~"\
	"7~7~~{c} {c} 1 173 53 7~8~8~~{c} {c} 1 173 53 8~9~9~~{c} {c} 1 173 53 9~"\
	"0~0~~{c} {c} 1 173 53 0~"\
	"C~0~~{c} {c} 1 173 53 -1~";
const char *NumberPassthrough = "*1~1~~{c} {e} 1 81~*2~2~~{c} {e} 1 82~*3~3~~{c} {e} 1 83~"\
	"*4~4~~{c} {e} 1 84~*5~5~~{c} {e} 1 85~*6~6~~{c} {e} 1 86~"\
	"*7~7~~{c} {e} 1 87~*8~8~~{c} {e} 1 88~*9~9~~{c} {e} 1 89~"\
	"*0~0~~{c} {e} 1 80~*#~0~~{c} {e} 1 41~";
const char *Help1 = "A~left~~{c} {c} 1 22 6 32~B~right~~{c} {c} 1 22 6 31~1~~~{c} {c} 1 22 6 0~";
const char *Help2 = "A~left~~{c} {c} 1 22 6 30~B~right~~{c} {c} 1 22 6 32~1~~~{c} {c} 1 22 6 0~";
const char *Help3 = "A~left~~{c} {c} 1 22 6 31~B~right~~{c} {c} 1 22 6 30~1~~~{c} {c} 1 22 6 0~";

MobileOrbiter::MobileOrbiter(class DCEMI_PS_VirtualDevices *pDCEMI_PS_VirtualDevices,class PlutoOrbiter *pPlutoOrbiter,string ID,class DeviceData_Router *pDevice) : 
		m_pDCEMI_PS_VirtualDevices(pDCEMI_PS_VirtualDevices), m_pPlutoOrbiter(pPlutoOrbiter), m_pDevice_CurrentDetected(NULL),m_sID(ID),m_pDevice_This(pDevice),
		m_EnterKeyAlarm(0), m_MoMutex("ir") 
{
	m_pRouter=m_pDCEMI_PS_VirtualDevices->m_pRouter;
	m_MoMutex.Init(NULL);
	m_bBuildingMenu=false;
	m_bMaskInput=m_bFollowMeLights=m_bFollowMeClimate=m_bFollowMeMedia=m_bFollowMeSecurity=m_bFollowMeTelecom=false;
	m_pLastEntgroup=NULL;
	m_pEntGroupAudioZone_LockedOn=NULL;
	m_pRoom_LockedOn=NULL;
	m_iLastSignalStrength=0;
}

void MobileOrbiter::ShowMenu(int MenuType,Message *pOriginatingMessage)
{
	int iOriginalLastMenuType = m_iLastMenuType;
	if( !m_pDevice_CurrentDetected )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Received ShowMenu with no detected device");
		return;
	}
	g_pPlutoLogger->Write(LV_STATUS,"ShowMenu %d %p",MenuType,pOriginatingMessage);

	PLUTO_SAFETY_LOCK(mo,m_MoMutex);
	m_bBuildingMenu=true;

	bool bShowingSameMenuAsBefore=false;

	if( MenuType==MENUTYPE_REFRESH )
	{
		MenuType=m_iLastMenuType;
		bShowingSameMenuAsBefore=true;
	}
	else
	{
		if( MenuType==m_iLastMenuType )
		{
			bShowingSameMenuAsBefore=true;
		}
		else
		{
			if( m_iLastMenuType==MENUTYPE_REMOTE_PLUTO_TV ||
				m_iLastMenuType==MENUTYPE_REMOTE_PLUTO_EPG ||
				m_iLastMenuType==MENUTYPE_REMOTE_NON_PLUTO_TV ||
				m_iLastMenuType==MENUTYPE_REMOTE_TUNERS ||
				m_iLastMenuType==MENUTYPE_REMOTE_NPTV_MORE ||
				m_iLastMenuType==MENUTYPE_REMOTE_PLAYLIST_ONE ||
				m_iLastMenuType==MENUTYPE_REMOTE_PLAYLIST_LIST ||
				m_iLastMenuType==MENUTYPE_REMOTE_DVD ||
				m_iLastMenuType==MENUTYPE_REMOTE_DVDMENU ||
				m_iLastMenuType==MENUTYPE_REMOTE_MEDIA_ADVANCED ||
				m_iLastMenuType==MENUTYPE_RESEND_IR)
			{
				m_pRouter->DispatchMessage(new Message(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device,DEVICEID_DCEROUTER,
						PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_MEDIA_REMOTE_CONST,2,
						COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(m_pPlutoOrbiter->m_pEntZone->m_PK_Device).c_str(),
						COMMANDPARAMETER_OnOff_CONST,"0"));
			}
		}
		if( MenuType==MENUTYPE_BACK ) // just refresh
			MenuType = m_iLastMenuType;
		else 
		{
			if( MenuType==MENUTYPE_REMOTE )
			{
				if( m_pPlutoOrbiter->m_pEntZone && m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream && m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream->m_PK_DesignObj_Remote )
					MenuType = m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream->m_PK_DesignObj_Remote;
				else
					MenuType=m_iLastMenuType; // Just refresh
			}
			
			if( m_iLastMenuType!=MenuType )
			{
				m_sCurrentInput="";
				m_bMaskInput=false;
				m_iLastMenuType = MenuType;
			}
		}
	}
	string sMenuList,sText=GlobalOptions,sTokens = "CI|" + (m_bMaskInput ? string("**************").substr(0,m_sCurrentInput.length()) : m_sCurrentInput) + "|";

	if( !bShowingSameMenuAsBefore && (MenuType==MENUTYPE_REMOTE_PLUTO_TV ||
		MenuType==MENUTYPE_REMOTE_PLUTO_EPG ||
		MenuType==MENUTYPE_REMOTE_NON_PLUTO_TV ||
		MenuType==MENUTYPE_REMOTE_TUNERS ||
		MenuType==MENUTYPE_REMOTE_NPTV_MORE ||
		MenuType==MENUTYPE_REMOTE_PLAYLIST_ONE ||
		MenuType==MENUTYPE_REMOTE_PLAYLIST_LIST ||
		MenuType==MENUTYPE_REMOTE_DVD ||
		MenuType==MENUTYPE_REMOTE_DVDMENU ||
		MenuType==MENUTYPE_REMOTE_MEDIA_ADVANCED ||
		MenuType==MENUTYPE_RESEND_IR) )
	{
		m_pRouter->DispatchMessage(new Message(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device,DEVICEID_DCEROUTER,
			PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_LINK_MEDIA_REMOTE_CONST,3,
			COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(m_pPlutoOrbiter->m_pEntZone->m_PK_Device).c_str(),
			COMMANDPARAMETER_OnOff_CONST,"1",
			COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(MenuType).c_str()));
	}

	switch(MenuType)
	{
	case MENUTYPE_MAINMENU:
		CreateMainMenu(sText,sTokens);
		break;
	case MENUTYPE_LIGHTING:
		CreateArrayMenu(sText,sTokens,C_ARRAY_LIGHTING_SCENARIOS_CONST);
		break;
	case MENUTYPE_MEDIA:
		CreatePlutoMediaMenu(sText,sTokens);
		break;
	case MENUTYPE_OTHER_MEDIA:
		CreateArrayMenu(sText,sTokens,C_ARRAY_ENTERTAINMENT_SCENARIOS_CONST);
		break;
	case MENUTYPE_CLIMATE:
		CreateArrayMenu(sText,sTokens,C_ARRAY_CLIMATE_SCENARIOS_CONST);
		break;
	case MENUTYPE_TELECOM:
		CreateArrayMenu(sText,sTokens,C_ARRAY_COMMUNICATIONS_SCENARIO_CONST);
		break;
	case MENUTYPE_SECURITY:
		CreateSecurityMenu(sText,sTokens);
		break;
	case MENUTYPE_OTHER:
		CreateArrayMenu(sText,sTokens,C_ARRAY_OTHER_SCENARIOS_CONST);
		break;
	case MENUTYPE_LOCATIONS:
		CreateLocationsMenu(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_MEDIA_FILES:
		CreateMediaFilesMenu(sText,sTokens,sMenuList,pOriginatingMessage);
		break;
	case MENUTYPE_REMOTE_PLUTO_TV:
		/* can't do this.  otherwise we get a pluto tv remote with np tv buttons
		if( !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream || !m_pPlutoOrbiter->m_pEntGroup->m_pCustomEPGView )
			CreateNonPlutoTVRemote(sText,sTokens);
		else
		*/
			CreatePlutoTVRemote(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_PLUTO_EPG:
//		if( !m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream || m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream->m_PK_DesignObj_Remote!=OBJECT_PLUTOTV )
			CreateNonPlutoTVRemote(sText,sTokens);
//		else
//			CreateEPGRemote(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_REMOTE_NPTV_MORE:
		CreateNonPlutoTVRemoteMore(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_NON_PLUTO_TV:
		CreateNonPlutoTVRemote(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_TUNERS:
		CreateNonPlutoTVTuners(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_REMOTE_PLAYLIST_ONE:
		CreatePlaylistOneRemote(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_PLAYLIST_LIST:
		CreatePlaylistListRemote(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_REMOTE_DVD:
		CreateDVDRemote(sText,sTokens);
		m_pRouter->DispatchMessage(new Message(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device,m_pPlutoOrbiter->m_pEntZone->m_PK_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_MENU_CAPTURE_CONST,3,
				COMMANDPARAMETER_PK_Device_CONST,StringUtils::itos(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device).c_str(),
				COMMANDPARAMETER_ID_CONST,"0",
				COMMANDPARAMETER_PK_DesignObj_CONST,(StringUtils::itos(MENUTYPE_REMOTE_DVD)+".0.0").c_str()));

	case MENUTYPE_REMOTE_DVDMENU:
		CreateDVDRemoteMenu(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_MEDIA_ADVANCED:
		CreateRemoteMediaAdvancedOptions(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_REMOTE_COMPUTING:
		CreateRemoteComputing(sText,sTokens);
		break;
	case MENUTYPE_REMOTE_WEB:
		CreateRemoteWeb(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_RESEND_IR:
		CreateRemoteMediaResendIR(sText,sTokens,sMenuList);
		break;
	case MENUTYPE_VIEW_CAMERA:
		CreateCameraViewMenu(sText,sTokens,pOriginatingMessage);
		break;
	case MENUTYPE_HELP1:
		sText += Help1;
		break;
	case MENUTYPE_HELP2:
		sText += Help2;
		break;
	case MENUTYPE_HELP3:
		sText += Help3;
		break;


	case MENUTYPE_JOESINVOICE:
		CreateJoesInvoiceMenu(sText,sTokens,pOriginatingMessage);
		break;
	case MENUTYPE_JOESPMTMETHOD:
		{
			sText += "1~1~~{c} {c} 1 22 6 992 53 " + m_sLastBasketID + " 5 1~";
			sText += "2~1~~{c} {c} 1 22 6 992 53 " + m_sLastBasketID + " 5 2~";
			sText += "3~1~~{c} {c} 1 22 6 992 53 " + m_sLastBasketID + " 5 3~";
			sText += "N~1~~{c} {c} 1 22 6 990 53 " + m_sLastBasketID + " 5 3~";
		}
		break;
	case MENUTYPE_JOESPINCODE:
	case MENUTYPE_JOESBADPINCODE:
		CreateJoesPinMenu(sText,sTokens,pOriginatingMessage);
		break;
	case MENUTYPE_JOESPAYMENTOK:
		if( m_sLastPin=="1234" )
		{
			sText += "Y~1~~{c} {c} 1 22 6 0~";
            string out = "wget -q -O - \"http://hlcs2025s/pos/Invoice/AddPlutoPayment.aspx?BasketHeader=" + m_sLastBasketID + "&Amount=" + m_sLastAmount + "\"";
	g_pPlutoLogger->Write(LV_CRITICAL,"sending %s",out.c_str());
#ifndef WIN32
	FILE* f = popen(out.c_str(), "r");
	g_pPlutoLogger->Write(LV_CRITICAL,"postpayment was: %p",f);
	if( f )
		fclose(f);
#endif
		}
		else
		{
			MenuType=m_iLastMenuType=MENUTYPE_JOESBADPINCODE;
			CreateJoesPinMenu(sText,sTokens,pOriginatingMessage);
		}
		break;

	default:
		// We didn't get anything valid
		m_iLastMenuType=iOriginalLastMenuType;
		return;
	}

	for(int loop=0;loop<2;++loop)
	{
		string *psText;
		if( loop==0 )
			psText = &sText;
		else
			psText = &sMenuList;

		string::size_type s;
		if( m_pPlutoOrbiter->m_pEntZone )
		{
			while( (s=psText->find("{e}"))!=string::npos )
			{
				psText->replace(s,3,StringUtils::itos(m_pPlutoOrbiter->m_pEntZone->m_PK_Device));
			}
		}
		string sController = StringUtils::itos(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device);
		while( (s=psText->find("{c}"))!=string::npos )
			psText->replace(s,3,sController);
		while( (s=psText->find("{mod}"))!=string::npos )
			psText->replace(s,5,StringUtils::itos(m_pDevice_CurrentDetected->m_iPK_Device));
	}

	m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER,m_pDevice_CurrentDetected->m_iPK_Device,PRIORITY_NORMAL,
		MESSAGETYPE_COMMAND,COMMAND_SHOW_MENU_CONST,8,
		C_COMMANDPARAMETER_TOKENS_CONST,sTokens.c_str(),
		COMMANDPARAMETER_Text_CONST,sText.c_str(),
		COMMANDPARAMETER_ID_CONST,m_sID.c_str(),
		COMMANDPARAMETER_Type_CONST,StringUtils::itos(m_iLastMenuType).c_str(),
		C_COMMANDPARAMETER_IMAGE_WIDTH_CONST,m_sWidth.c_str(),
		C_COMMANDPARAMETER_IMAGE_HEIGHT_CONST,m_sHeight.c_str(),
		C_COMMANDPARAMETER_LIST_CONST,sMenuList.c_str(),
		// TODO!  Get this value out of the associated phone
		C_COMMANDPARAMETER_PATH_CONST, "Nokia36x0"));

	m_bBuildingMenu=false;

}

void MobileOrbiter::CreateMainMenu(string &sText,string &sTokens)
{
	AddNowPlayingInfo(sTokens,false,12);
	sText += MainMenuOptions;
	sText += StopStream;
	if( m_pDevice_This->m_pRoom )
	{
		vector<string> vectLines;
		StringUtils::BreakIntoLines(m_pDevice_This->m_pRoom->m_Description,&vectLines,12);
		for(int line=0;line<2;++line)
		{
			if( line>=(int) vectLines.size() )
				break;

			sTokens += "DD_" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
		}
	}

	if( m_bFollowMeLights )
		sTokens += m_pEntGroupAudioZone_LockedOn ? "FM1|no_follow.png|" : "FM1|follow_me.png|";
	if( m_bFollowMeMedia )
		sTokens += m_pEntGroupAudioZone_LockedOn ? "FM2|no_follow.png|" : "FM2|follow_me.png|";
	if( m_bFollowMeClimate )
		sTokens += m_pEntGroupAudioZone_LockedOn ? "FM3|no_follow.png|" : "FM3|follow_me.png|";
	if( m_bFollowMeTelecom )
		sTokens += m_pEntGroupAudioZone_LockedOn ? "FM4|no_follow.png|" : "FM4|follow_me.png|";
	if( m_bFollowMeSecurity )
		sTokens += m_pEntGroupAudioZone_LockedOn ? "FM5|no_follow.png|" : "FM5|follow_me.png|";

// This is a | delimited list of the action groups for shortcuts 7,8,9,0,*,#
	string sMainMenuShortcuts = "6441,6479,6442,6643,6478,6477";

//	DeviceRelation *pDR = NULL; // TODO - 7/20/04 m_pDevice_This->m_mapDevices_Find(m_pDevice_CurrentDetected->m_iPK_Device);

	sText += "1~~~{c} {c} 1 22 6 1~2~~~{c} {c} 1 22 6 2~3~~~{c} {c} 1 22 6 3~"\
			 "4~~~{c} {c} 1 22 6 4~5~~~{c} {c} 1 22 6 5~6~~~{c} {c} 1 22 6 6~";
	
	//	if( pDR )
//		sMainMenuShortcuts=pDR->sParms;  // This will be the main menu stuff

	// The main menu has slots for six shortcuts for the number 6,7,8,9,0,*,#

	string::size_type pos=0;
	for(int i=0;i<6;++i)
	{
		// when i is 0, we're doing the shortcut for 6, 3 is 9, 4 is 0, 5 is *, 6 is #
		
		char ScanCode[2]; // The scan code in the file
		ScanCode[1]=0;
		if( i<3 )
			ScanCode[0] = i+'7';
		else if( i==3 )
			ScanCode[0] = '0';
		else if( i==4 )
			ScanCode[0] = '*';  // check ??
		else
			ScanCode[0] = '#';  // check ??

		int PK_CommandGroup = atoi(StringUtils::Tokenize(sMainMenuShortcuts,",",pos).c_str());
		if( !PK_CommandGroup )
			break;

        CommandGroup *pCG = m_pRouter->mapCommandGroup_Find(PK_CommandGroup);
		if( pCG )
		{
			// COMMAND_EXECUTE_COMMAND_GROUP_CONST = 210
			// COMMANDPARAMETER_PK_CommandGroup_CONST = 75
			// Don't worry about the description.  We're using the token
			// TODO: Add icons
			sText += "*" + string(ScanCode) + "~~~{c} 0 1 210 75 " + StringUtils::itos(PK_CommandGroup) + "~";

			vector<string> vectLines;
			StringUtils::BreakIntoLines(pCG->m_Description,&vectLines,12);
			for(int line=0;line<2;++line)
			{
				if( line>=(int) vectLines.size() )
					break;

				sTokens += "SC" + string(ScanCode) + "_" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
			}
		}
	}

	return;
}

class LocationInfo
{
public:
	string m_sDescription;
	int m_PK_EntGroup;
	int m_PK_Room;

	LocationInfo(string Description,int PK_EntGroup,int PK_Room) : 
		m_sDescription(Description), m_PK_EntGroup(PK_EntGroup), m_PK_Room(PK_Room) {}
};

static bool LocationInfoCompare(LocationInfo *x, LocationInfo *y)
{
	return x->m_sDescription<y->m_sDescription;
}

void MobileOrbiter::CreateLocationsMenu(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += MainMenu;
	sText += PagableList;

	string::size_type pos=0;

	list<LocationInfo *> listLocations;

	sMenuList = StringUtils::itos(MENUITEM_LOCATIONS) + "\t0\t";
	sMenuList += /*value not important*/ /*"\tauto\t{c} -12 1 412 5 0 77 0\t";
	map<int,DCERoom *>::iterator itRooms;
	for(itRooms=m_pRouter->m_mapRoom.begin();itRooms!=m_pRouter->m_mapRoom.end();++itRooms)
	{
		DCERoom *pRoom = (*itRooms).second;
		if( !pRoom )
			continue;

		DeviceList listAudioZonesInRoom;
		DeviceList::iterator itDevices;
		for(itDevices=pRoom->m_listDevices.begin();itDevices!=pRoom->m_listDevices.end();++itDevices)
		{
			DeviceData_Router *pDevice = (*itDevices);
			if( !pDevice || (pDevice->m_iPK_DeviceTemplate!=DEVICETEMPLATE_Entertain_Unit_CONST && pDevice->m_iPK_DeviceCategory!=DEVICECATEGORY_AUDIO_ZONE_CONST) )
				continue;
			listAudioZonesInRoom.push_back(pDevice);
		}

		if( listAudioZonesInRoom.size()==0 )
		{
			// We've got a room, but no a/v
			listLocations.push_back(new LocationInfo(pRoom->m_Description,0,pRoom->m_PK_Room));
		}
		else
		{
			for(itDevices=listAudioZonesInRoom.begin();itDevices!=listAudioZonesInRoom.end();++itDevices)
			{
				DeviceData_Router *pDevice = (*itDevices);
				if( listAudioZonesInRoom.size()==1 )
					listLocations.push_back(new LocationInfo(pRoom->m_Description,pDevice->m_iPK_Device,pRoom->m_PK_Room));
				else
					listLocations.push_back(new LocationInfo(pRoom->m_Description + "/" + pDevice->m_sDescription,
						pDevice->m_iPK_Device,pRoom->m_PK_Room));
			}
		}
	}
	listLocations.sort(LocationInfoCompare);
	list<LocationInfo *>::iterator itLocations;
	
	for(itLocations=listLocations.begin();itLocations!=listLocations.end();++itLocations)
	{
		LocationInfo *pLI = *itLocations;
		sMenuList += /*value not important*/ /*"\t" + pLI->m_sDescription + "\t"\
			"{c} -12 1 412 5 " + StringUtils::itos(pLI->m_PK_EntGroup) + " 77 " + StringUtils::itos(pLI->m_PK_Room) + "\t";
		delete pLI;  // It was just temporary
	}
*/
}

void MobileOrbiter::CreateArrayMenu(string &sText,string &sTokens,int PK_C_Array)
{
	/*
	sText += MainMenu;
	sText += StopStream;

	vector<CommandGroup *> *vectCommandGroups;
	if( PK_C_Array==C_ARRAY_ENTERTAINMENT_SCENARIOS_CONST )
	{
//		vectCommandGroups=&m_pPlutoOrbiter->m_pEntZone->m_ptrDevice->m_vectCommandGroupsForEntGroup;
		sText += VolumeUpDown;
	}
	else
	{
		if( PK_C_Array==C_ARRAY_LIGHTING_SCENARIOS_CONST )
			sText += LightsUpDown;
		else if( PK_C_Array==C_ARRAY_CLIMATE_SCENARIOS_CONST && m_pDevice_CurrentDetected->m_pRoom )
			sTokens += "TEMP|" + StringUtils::itos(m_pDevice_CurrentDetected->m_pRoom->m_iTemperature) + "|";
		
		if( !m_pDevice_This->m_pRoom )
		{
			g_pPlutoLogger->Write(LV_WARNING,"No room for detector %d",m_pDevice_CurrentDetected->m_iPK_Device);
			return;
		}
		vectCommandGroups=&m_pDevice_This->m_pRoom ->m_vectCommandGroups;
	}

	if( !vectCommandGroups )
	{
		g_pPlutoLogger->Write(LV_WARNING,"No action groups for array %d device %d",PK_C_Array,m_pDevice_CurrentDetected->m_iPK_Device);
		return;
	}

	int ShortCutCounter=0;
	int Key;
	for(int ags=0;ags<(int) vectCommandGroups->size();ags++)
	{
		if( ShortCutCounter>=12 )
			break;
		CommandGroup *pAG = (*vectCommandGroups)[ags];
		if( pAG->m_PK_C_Array!=PK_C_Array || pAG->m_RecognizedTypes==agrt_NoCommands )
			continue;
		if( PK_C_Array==C_ARRAY_SECURITY_SCENARIOS_CONST && 
			(pAG->m_RecognizedTypes==agrt_CameraQuadView || pAG->m_RecognizedTypes==agrt_CameraSingleView) )
			continue;
		else if ( PK_C_Array==C_ARRAY_COMMUNICATIONS_SCENARIO_CONST && 
			(pAG->m_RecognizedTypes==agrt_UserStatus || pAG->m_RecognizedTypes==agrt_MakeACall) )
			continue;
		else if( pAG->m_Description.substr(0,3)=="<%=" )
			continue;
		Key=MapShortCutCounterToKey(ShortCutCounter);
		++ShortCutCounter;
		sText += StringUtils::itos(Key) + "~~~{c} 0 1 210 75 " + StringUtils::itos(pAG->m_PK_CommandGroup) + "~";
		vector<string> vectLines;
		StringUtils::BreakIntoLines(pAG->m_Description,&vectLines,12);
		for(int line=0;line<2;++line)
		{
			if( line>=(int) vectLines.size() )
				break;

			sTokens += "SC" + StringUtils::itos(Key) + "_" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
		}
	}
*/
}

void MobileOrbiter::CreateSecurityMenu(string &sText,string &sTokens)
{
	/*
	sText += MainMenu;
	sText += StopStream;
	sText += SecurityMenu;

	int ShortCutCounter=7;

	DeviceList *pList=m_pCore->m_pPlutoEvents->m_mapDeviceCategories[DEVICECATEGORY_CAMERA_CONST];
	if( pList )
	{
		int NumCameras=0;
		DeviceList::iterator itCameras;
		for(itCameras=pList->begin();itCameras!=pList->end();++itCameras)
		{
			int Key=MapShortCutCounterToKey(ShortCutCounter);
			if( ShortCutCounter>=12 && NumCameras<(int) pList->size() )
			{
				// Add a 'more cameras' buttons
				sText += StringUtils::itos(Key) + "~~~{c} {c} 1 22 6 1414~";
				sTokens += "SC12_1|all|SC12_2|cameras|";
				break;
			}
			else
			{
				DeviceData_Router *pCamera = *itCameras;
				sText += StringUtils::itos(Key) + "~~~{c} {c} 1 22 6 1405 53 " + StringUtils::itos(pCamera->m_iPK_Device) + "~";
				vector<string> vectLines;
				StringUtils::BreakIntoLines(pCamera->m_sDescription,&vectLines,12);
				for(int line=0;line<2;++line)
				{
					if( line>=(int) vectLines.size() )
						break;

					sTokens += "SC" + StringUtils::itos(Key) + "_" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
				}  
				++ShortCutCounter;
			}
		}
	}
*/
}

void MobileOrbiter::CreatePlutoMediaMenu(string &sText,string &sTokens)
{
	sText += MainMenu;
	sText += StopStream;
	sText += VolumeUpDown;

	// 1=TV, 2=RECORDED TV, 3=MOVIES, 4=MUSIC, 5=RADIO, 6=START DISC
	// 7=PICTURES, 8=HOME VIDEOS, 9=BURNING, *=COMPUTING 0=PLAYLISTS #=OTHER
	sText += "1~~~{c} -12 1 345 5 {e} 62 1~2~~~{c} {c} 1 22 6 2071 53 11~3~~~{c} {c} 1 22 6 2071 53 13~"\
			 "4~~~{c} {c} 1 22 6 2071 53 12~5~~~{c} {c} 1 999 RADIO~6~~~{c} -12 1 345 5 {e} 62 2~"\
			 "7~~~{c} {c} 1 22 6 2071 53 pic~8~~~{c} {c} 1 22 6 2071 53 pic~9~~~{c} {c} 1 burn~"\
			 "*~~~{c} {c} 1 22 6 1981~0~~~{c} {c} 1 22 6 2071 53 16~#~~~{c} {c} 1 22 6 2071 53 11~";
}

void MobileOrbiter::CreateMediaFilesMenu(string &sText,string &sTokens,string &sMenuList,Message *pMessage)
{
	sText += MainMenu;
	sText += StopStream;
	sText += MediaFilesMenu;
	sText += GotoAdvancedMedia;
	if( !m_pPlutoOrbiter->m_pOCUser || !pMessage )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"No user/message specified for MO: %d %p %p",
			m_pDevice_This->m_iPK_Device,m_pPlutoOrbiter->m_pOCUser,pMessage);
		return;
	}
	sText += TypeAndSeek;

	DCEMI_PS_VirtualDevices::listDCEFileInfo *listFileNames = new DCEMI_PS_VirtualDevices::listDCEFileInfo();
	string::size_type pos=0;

	string ID = pMessage->m_Parameters[COMMANDPARAMETER_ID_CONST];
	string Path = pMessage->m_Parameters[C_COMMANDPARAMETER_PATH_CONST];

	if( Path.length()>0 )
		sTokens+="PATH|" + FileUtils::FilenameWithoutPath(Path) + "|";
	else
	{
		/*
		switch( atoi(ID.c_str()) )
		{
			case USING_ID_LIBRARY_RECORDED_TV:
		        sTokens+="PATH|recorded tv|";
				break;
			case USING_ID_LIBRARY_MUSIC:
		        sTokens+="PATH|music|";
				break;
			case USING_ID_LIBRARY_MOVIES:
		        sTokens+="PATH|movies|";
				break;
			case USING_ID_LIBRARY_PICTURES:
		        sTokens+="PATH|pictures|";
				break;
			case USING_ID_LIBRARY_HOMEVIDEOS:
		        sTokens+="PATH|home videos|";
				break;
			case USING_ID_LIBRARY_PLAYLISTS:
		        sTokens+="PATH|playlists|";
				break;
			default:
				sTokens+="PATH|media|";
				break;
		}
*/
	}

	string Parms = ID;// + "," + Path + "," + StringUtils::itos(m_pPlutoOrbiter->m_pOCUser->PK_Users);
//  TODO	m_pCore->m_pDataGridDevice->PopulateWithFileList("FILES_" + StringUtils::itos(m_pDevice_This->m_iPK_Device),Parms,pos,NULL,listFileNames);

	sMenuList = StringUtils::itos(MENUITEM_FILELIST) + "\t0\t";
    
	vectMenuItem vmi;
	for(DCEMI_PS_VirtualDevices::listDCEFileInfo::iterator itFI=listFileNames->begin();itFI!=listFileNames->end();++itFI)
	{
		DCEMI_PS_VirtualDevices::DCEFileInfo *fi = (*itFI);
		sMenuList += /*value not important*/"\t" + fi->m_sDisplayName + "\t";
		
		if( fi->m_bIsDirectory )
			sMenuList += "{c} {c} 1 22 6 2071 53 " + ID + " 28 \"" + fi->m_sPath + fi->m_sRealFileName + "/\"\t";
		else
			sMenuList += "{c} -12 1 345 5 {e} 45 \"" + fi->m_sPath + fi->m_sRealFileName + "\"\t";
	}
}

void MobileOrbiter::CreatePlutoTVRemote(string &sText,string &sTokens)
{
/*
	sText += "C~~~{c} {c} 1 22 6 2125~";
	sText += SkipUpDownBackFwdNav; 
	sText += StopStream;
	sText += SimulateKeyPresses;
	sText += GoToNPTV;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;

	if( m_pPlutoOrbiter->m_pEntZone && m_pPlutoOrbiter->m_pEntZone->m_pDCEMediaStream && m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->pProvider_Station__Schedule )
	{
		EPGProvider_Station__Schedule *p = m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->pProvider_Station__Schedule;
		sTokens += "CH|" + StringUtils::itos(p->m_pChannel->m_tmsChannel) + "|"\
			"CN|" + p->m_pChannel->m_ChannelName + "|";

		vector<string> vectLines;
		StringUtils::BreakIntoLines(p->m_Title20,&vectLines,25);
		string::size_type pos=0;
		for(int line=0;line<(int) vectLines.size();line++)
		{
			sTokens += "SN" + StringUtils::itos(line) + "|" + vectLines[line] + "|";
		}
		if( p->m_pChannel->m_sIconFileName.length()>0 )
			sTokens += "IF|" + p->m_pChannel->m_sIconFileName + "|";
		
		if( p->m_pXMLTVSchedule )
		{
			vectLines.clear();
			StringUtils::BreakIntoLines(p->m_pXMLTVSchedule->m_sDescription,&vectLines,35);
			string::size_type pos=0;
			for(int line=0;line<(int) vectLines.size();line++)
			{
				sTokens += "SD" + StringUtils::itos(line) + "|" + vectLines[line] + "|";
			}
		}

		// The following is copied from ::SetShowInfo  -- should have a common source
		time_t tNow = time(NULL);
		struct tm *tmNow = localtime(&tNow);
		struct tm *tmStart = localtime(&p->m_StartTime);
		string sStartDate;
		if( tmStart->tm_year==tmNow->tm_year && tmStart->tm_yday==tmNow->tm_yday )
			sStartDate="today";
		else if( tmStart->tm_yday==tmNow->tm_yday+1 )
			sStartDate="tom";
		else
			sStartDate=StringUtils::itos(tmStart->tm_mon+1) + "/" + StringUtils::itos(tmStart->tm_mday);

		string StartTime = StringUtils::itos(tmStart->tm_hour==0 ? 12 : (tmStart->tm_hour>13 ? tmStart->tm_hour-12 : tmStart->tm_hour)) + ":" + 
			(tmStart->tm_min<10 ? "0" : "") + StringUtils::itos(tmStart->tm_min) + (tmStart->tm_hour>=12 ? "p" : "a");

		time_t tStop = p->m_StartTime+p->m_DurationMinutes*60;
		struct tm *tmStop = localtime(&tStop);

		string StopTime = StringUtils::itos(tmStop->tm_hour==0 ? 12 : (tmStop->tm_hour>13 ? tmStop->tm_hour-12 : tmStop->tm_hour)) + ":" + 
			(tmStop->tm_min<10 ? "0" : "") + StringUtils::itos(tmStop->tm_min) + (tmStop->tm_hour>11 ? "p" : "a");

		sTokens += "ST|" + StartTime + "-" + StopTime + "|";
	}
*/
}


void MobileOrbiter::CreateEPGRemote(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += GoToNPTV;
	sText += StopStream;
	sText += PagableList;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;
	sText += MainMenu;

	AddNowPlayingInfo(sTokens,false);
	EPGShowInfoVect vectItems;
	string::size_type pos=0;
*/
	
/*
		if( pXMLTVSchedule )
			PK_Provider_Station = pXMLTVSchedule->m_pStation->m_PK_Provider_Station;
		else
			PK_Provider_Station = atoi(row[COL_PROVIDERSTATION]);


		int PK_Schedule = pXMLTVSchedule ? pXMLTVSchedule->m_iPK_Schedule : atoi(row[COL_PK_SCHEDULE]);
		int PK_Program = pXMLTVSchedule ? pXMLTVSchedule->m_pProgram->m_iPK_Program : atoi(row[COL_PK_PROGRAM]);
		string CellID = StringUtils::itos(PK_Schedule);
		CellID += ",";
		CellID += StringUtils::itos(PK_Provider_Station);
*/

	int CurrentRow = 1;/*TODO m_pCore->m_pDataGridDevice->PopulateWithCurrentShows("CURSH_" + StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->m_PK_EntGroup),
		"1," + StringUtils::itos(m_pDevice_This->m_iPK_Device) + "," + 
		StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->pProvider_Station__Schedule_CurrentlyWatching->m_PK_Schedule) + "," +
		StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->pProvider_Station__Schedule_CurrentlyWatching->m_PK_Provider_Station),
		pos,&vectItems);*/
/*
	sMenuList = StringUtils::itos(MENUITEM_EPG) + "\t" + StringUtils::itos(CurrentRow) + "\t";

	for(int i=0;i<(int) vectItems.size();++i)
	{
		EPGShowInfo *si = vectItems[i];
		sMenuList += //value not important
			"\t" + si->m_sText + "\t"\
			"0 168 1 209 44 " + si->m_sValue + ",{c},0 62 2 66 EPG_" + "{e}\t";
		delete si;  // It was just temporary
	}
*/
}

void MobileOrbiter::CreateNonPlutoTVRemoteMore(string &sText,string &sTokens)
{
	/*
	sText += GoToPlutoTV;
	sText += UDLREnterNav;
	sText += StopStream;
	sText += MoreNPTVOptions;
	sText += MainMenu;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;
	if( m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
	{
		DeviceData_Router *pD = m_pCore->m_pPlutoEvents->m_mapDevice_Find(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo);
		if( pD )
			sTokens += "TD|" + pD->m_sDescription + "|";
	}
	*/
}

void MobileOrbiter::CreateNonPlutoTVRemote(string &sText,string &sTokens)
{
	/*
	sText += GoToPlutoTV;
	sText += SkipUpDownBackFwdNav;
	sText += StopStream;
	sText += NumberPassthrough;
	sText += NPTVOptions;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;

	if( m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
	{
		DeviceData_Router *pD = m_pCore->m_pPlutoEvents->m_mapDevice_Find(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo);
		if( pD )
			sTokens += "TD|" + pD->m_sDescription + "|";
	}
	*/
}

void MobileOrbiter::CreatePlaylistOneRemote(string &sText,string &sTokens)
{
	sText += "C~~~{c} {c} 1 22 6 24~";
	sText += SkipUpDownBackFwdNav; // Goto playlist list remote
	sText += StopStream;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;
	AddNowPlayingInfo(sTokens,true);
}

void MobileOrbiter::CreatePlaylistListRemote(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += MainMenu;
	sText += SkipUpDownBackFwdNav;
	sText += StopStream;
	sText += PagableList;
	sText += VolumeUpDown;

	if( !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
		return;
	AddNowPlayingInfo(sTokens,false);
	sMenuList = StringUtils::itos(MENUITEM_PLAYLIST) + "\t0\t";
    vector<string> vectItems;
	string::size_type pos=0;
//	m_pCore->m_pDataGridDevice->PopulateWithPlaylist("",StringUtils::itos(m_pDevice_This->m_iPK_Device),pos,&vectItems);

	for(int i=0;i<(int) vectItems.size();++i)
	{
		sMenuList += //value not important
			"\t" + vectItems[i] + "\t"\
			"-12 " + StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo) + " 1 279 53 " + StringUtils::itos(i+1) + "\t";  // Playlist is 1 based
	}
*/
}

void MobileOrbiter::CreateDVDRemote(string &sText,string &sTokens)
{
	sText += MainMenu;
	sText += StopStream;
	sText += SkipUpDownBackFwdNav;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;
	sText += DVDRemote;

	AddNowPlayingInfo(sTokens,true);
}

void MobileOrbiter::CreateDVDRemoteMenu(string &sText,string &sTokens)
{
	sText += MainMenu;
	sText += StopStream;
	sText += UDLREnterNav;
	sText += VolumeUpDown;
	sText += GotoAdvancedMedia;
	AddNowPlayingInfo(sTokens,true);
}

void MobileOrbiter::CreateRemoteComputing(string &sText,string &sTokens)
{
	sText += MainMenu;
	sText += VolumeUpDown;
	sText += Computing;

	string query = ""; //string("SELECT URL,Description from WEB_Favorite WHERE FK_Users=") + (m_pPlutoOrbiter->m_pOCUser ? StringUtils::itos(m_pPlutoOrbiter->m_pOCUser->PK_Users) : "0") + " OR FK_Users=0 ORDER BY OrderNum";
	PlutoSqlResult result;
	MYSQL_ROW row=NULL;
	int iRow=0;

	if( (result.r=m_pDCEMI_PS_VirtualDevices->mysql_query_result(query)) )
	{
		for(int i=0;i<9 && (row=mysql_fetch_row(result.r));++i)
		{
			// when i is 0, we're doing the shortcut for 4, 5 is 9, 6 is 0, 7 is *, 8 is #
			
			char ScanCode; // The scan code in the file
			if( i<6 )
				ScanCode = i+'4';
			else if( i==6 )
				ScanCode = '0';
			else if( i==7 )
				ScanCode = '*';
			else
				ScanCode = '#';

			sText += "*" + StringUtils::itos(ScanCode) + "~~~{c} -23 1 320 27 \"" + row[0] + "\"~";

			string sDescription = row[1];
			vector<string> vectLines;
			StringUtils::BreakIntoLines(sDescription,&vectLines,12);
			for(int line=0;line<2;++line)
			{
				if( line>=(int) vectLines.size() )
					break;

				sTokens += "SC" + StringUtils::itos(ScanCode) + "_" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
			}
		}
	}
}

void MobileOrbiter::CreateRemoteWeb(string &sText,string &sTokens,string &sMenuList)
{
	sText += MainMenu;
	sText += StopStream;
	sText += VolumeUpDown;
	sText += PagableList;

}

void MobileOrbiter::CreateNonPlutoTVTuners(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += UDLREnterNav;
	sText += StopStream;
	sText += PagableList;
	sText += VolumeUpDown;
	sText += MainMenu;

	sMenuList = StringUtils::itos(MENUITEM_TUNERS) + "\t0\t";

	for(int i=0;i<(int) m_pPlutoOrbiter->m_pEntGroup->m_vectDevice_AV.size();++i)
	{
		DeviceData_Router *pDevice = m_pPlutoOrbiter->m_pEntGroup->m_vectDevice_AV[i];
		if( pDevice->m_iPK_DeviceCategory==DEVICECATEGORY_SATELLITE_CONST )
		{
			sMenuList += /*value not important*/ /*"\t" + pDevice->m_sDescription + "\t{c} {e} 1 405 5 {e} 42 1 6 2427 36 " + 
				StringUtils::itos(pDevice->m_iPK_Device) + "\t";
		}
	}
*/
}

void MobileOrbiter::CreateRemoteMediaAdvancedOptions(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += UDLREnterNav;
	sText += StopStream;
	sText += PagableList;
	sText += VolumeUpDown;
	sText += GotoResendIR;
	sText += ReturnToRemote;
	sText += MainMenu;

	if( !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"called advanced options for %d with no stream",
			m_pDevice_This->m_iPK_Device);
		return;
	}

	sMenuList = StringUtils::itos(MENUITEM_EXT_ACTIONS) + "\t0\t";
	list<class ExtraCommand *> listCommands;

	string::size_type pos=0;

	m_pCore->m_pDataGridDevice->PopulateWithSpecialCommands("",
		StringUtils::itos(m_pDevice_This->m_iPK_Device) + "," + StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo),
		pos,&listCommands);

	list<class ExtraCommand *>::iterator itList;
    for(itList=listCommands.begin();itList!=listCommands.end();++itList)
	{
		ExtraCommand *pEA = *itList;
		sMenuList += /*value not important*/   /*"\t" + pEA->m_sDescription + "\t" +
			StringUtils::itos(pEA->m_pMessage->m_DeviceIDFrom) + " " + StringUtils::itos(pEA->m_pMessage->m_DeviceIDTo) +
			" 1 " + StringUtils::itos(pEA->m_pMessage->m_ID) ;
		map<int, string>::iterator itParms;
		for(itParms=pEA->m_pMessage->m_Parameters.begin();itParms!=pEA->m_pMessage->m_Parameters.end();++itParms)
		{
			sMenuList += " " + StringUtils::itos((*itParms).first) + " \"" + (*itParms).second + "\"";
		}
		sMenuList += "\t";
	}
	*/
}

void MobileOrbiter::CreateRemoteMediaResendIR(string &sText,string &sTokens,string &sMenuList)
{
	/*
	sText += MainMenu;
	sText += UDLREnterNav;
	sText += StopStream;
	sText += PagableList;
	sText += VolumeUpDown;
	sText += ReturnToRemote;
	sText += GotoAdvancedMediaFromIR;

	if( !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"called advanced options for %d with no stream",
			m_pDevice_This->m_iPK_Device);
		return;
	}

	sMenuList = StringUtils::itos(MENUITEM_EXT_ACTIONS) + "\t0\t";
	list<class ExtraCommand *> listCommands;

	string::size_type pos=0;

	m_pCore->m_pDataGridDevice->PopulateWithResetAV("",
		StringUtils::itos(m_pDevice_This->m_iPK_Device) + "," + StringUtils::itos(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_PK_DeviceToSendTo),
		pos,NULL,&listCommands);

	list<class ExtraCommand *>::iterator itList;
    for(itList=listCommands.begin();itList!=listCommands.end();++itList)
	{
		ExtraCommand *pEA = *itList;
		sMenuList += /*value not important*/   /*"\t" + pEA->m_sDescription + "\t" +
			StringUtils::itos(pEA->m_pMessage->m_DeviceIDFrom) + " " + StringUtils::itos(pEA->m_pMessage->m_DeviceIDTo) +
			" 1 " + StringUtils::itos(pEA->m_pMessage->m_ID) ;
		map<int, string>::iterator itParms;
		for(itParms=pEA->m_pMessage->m_Parameters.begin();itParms!=pEA->m_pMessage->m_Parameters.end();++itParms)
		{
			sMenuList += " " + StringUtils::itos((*itParms).first) + " \"" + (*itParms).second + "\"";
		}
		sMenuList += "\t";
	}
*/

}

void MobileOrbiter::CreateCameraViewMenu(string &sText,string &sTokens,Message *pMessage)
{
	/*
	sText += CameraMenu;
	int PK_Device = atoi(pMessage->m_Parameters[COMMANDPARAMETER_ID_CONST].c_str());
	DeviceData_Router *pDevice = m_pCore->m_pPlutoEvents->m_mapDevice_Find(PK_Device);
	if( !pDevice )
		return;

	sText += "P~refresh~~{c} {c} 1 22 6 -3 53 " + StringUtils::itos(PK_Device) + "~";
	sTokens += "CAMD|" + pDevice->m_sDescription + "|";
	sTokens += "CAMN|" + StringUtils::itos(pDevice->m_iPK_Device) + "|";
	*/
}


void MobileOrbiter::AddNowPlayingInfo(string &sTokens,bool bWithPicture,int Lines)
{
	/*
	if( !m_pPlutoOrbiter->m_pEntGroup )
		return;
	if( m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream && !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_bShuttingDown )
	{
		vector<string> vectLines;
		StringUtils::BreakIntoLines(m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_sShortSummary,&vectLines,Lines);
		for(int line=0;line<2;++line)
		{
			if( line>=(int) vectLines.size() )
				break;

			sTokens += "NP" + StringUtils::itos(line+1) + "|" + vectLines[line] + "|";
		}

		if( bWithPicture && m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_sPictureFile.length()>0 )
		{
#ifdef WIN32
			string::size_type s;
			string File = m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_sPictureFile;
			while( (s=File.find("Z:/"))!=string::npos )
				File.replace(s,3,"/home/public/data/");
			while( (s=File.find("Z:\\"))!=string::npos )
				File.replace(s,3,"/home/public/data/");
			while( (s=File.find('\\'))!=string::npos )
				File.replace(s,1,"/");
			sTokens += "PIC|" + File + "|";
#else
			sTokens += "PIC|" + m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream->m_sPictureFile + "|";
#endif
		}
	}
*/
}

void MobileOrbiter::Keypress(string Key)
{
	/*
	switch(m_iLastMenuType)
	{
	case MENUTYPE_REMOTE_PLUTO_TV:
		if( m_EnterKeyAlarm )
			m_pRouter->m_pAlarmManager->CancelAlarm(m_EnterKeyAlarm);
		if( Key=="ENTER" )
		{
			if( m_pPlutoOrbiter->m_pEntGroup )
				m_pRouter->DispatchMessage(new Message(m_pPlutoOrbiter->m_pDeviceData_Router->m_iPK_Device,
					m_pPlutoOrbiter->m_pEntGroup->m_PK_EntGroup,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
					COMMAND_AV_CHANNEL_NUMBER_CONST,1,C_COMMANDPARAMETER_ABSOLUTE_CHANNEL_CONST,m_sCurrentInput.c_str()));
			m_sCurrentInput="";
		}
		else
		{
			m_sCurrentInput+=Key;
			m_EnterKeyAlarm = m_pRouter->m_pAlarmManager->AddRelativeAlarm(2,this,TIMER_DOENTER, NULL);
		}
		ShowMenu(m_iLastMenuType,NULL);
		break;
	case MENUTYPE_JOESINVOICE:
		{
			string::size_type s;
			while( (s=m_sCurrentInput.find("."))!=string::npos )
				m_sCurrentInput.replace(s,1,"");
			if( Key=="-1" )
			{
				if( m_sCurrentInput.length()>0 )
					m_sCurrentInput = m_sCurrentInput.substr(0,m_sCurrentInput.length()-1);
			}
			else
				m_sCurrentInput += Key;

			if( m_sCurrentInput[0]=='0' )
			{
				m_sCurrentInput = m_sCurrentInput.substr(1);
			}
			if( m_sCurrentInput.length()==1 )
				m_sCurrentInput = ".0" + m_sCurrentInput;
			else if( m_sCurrentInput.length()==2 )
				m_sCurrentInput = "." + m_sCurrentInput;
			else if( m_sCurrentInput.length()>2 )
				m_sCurrentInput = m_sCurrentInput.substr(0,m_sCurrentInput.length()-2) + "." + m_sCurrentInput.substr(m_sCurrentInput.length()-2);
			m_sLastAmount=m_sCurrentInput;
			ShowMenu(m_iLastMenuType,NULL);
		}
		break;
	case MENUTYPE_JOESPINCODE:
	case MENUTYPE_JOESBADPINCODE:
		if( Key=="-1" )
		{
			if( m_sCurrentInput.length()>0 )
				m_sCurrentInput = m_sCurrentInput.substr(0,m_sCurrentInput.length()-1);
		}
		else
			m_sCurrentInput += Key;

		m_sLastPin = m_sCurrentInput;
		ShowMenu(m_iLastMenuType,NULL);
		break;
	}
*/
}

void MobileOrbiter::AlarmCallback(int id, void* param)
{
	/*
	if( id==TIMER_DOENTER )
	{
		m_EnterKeyAlarm = 0;
		Keypress("ENTER");
	}
*/
}


char MobileOrbiter::MapShortCutCounterToKey(int ShortCutCounter)
{
	if( ShortCutCounter<9 )
		return '1'+ShortCutCounter;
	else if( ShortCutCounter==9 ) // Asterisk
		return '*';
	else if( ShortCutCounter==10 ) // 0
		return '0';
	else if( ShortCutCounter==11 ) // #
		return '#';

	return 'X';
}

void MobileOrbiter::RemovingAssocation()
{
	/*
	if( !m_pEntGroupAudioZone_LockedOn && m_bFollowMeLights )
	{
		if( m_pDevice_CurrentDetected && m_pDevice_CurrentDetected->m_pRoom )
		{
			bool bOrbitersInPriorRoom=false;
			// See if there are any orbiters in that room
			MobileOrbitersMap::iterator it;
			for( it=m_pCore->m_pPlutoEvents->m_mapMobileOrbiters.begin(); it!=m_pCore->m_pPlutoEvents->m_mapMobileOrbiters.end(); ++it )
			{
				DeviceData_Router *pD = (*it).second;
				if( pD && pD->m_pRoom==m_pDevice_CurrentDetected->m_pRoom && pD->m_pMobileOrbiter!=this )
				{
					bOrbitersInPriorRoom=true;
					break;
				}
			}

			if( !bOrbitersInPriorRoom )
			{
				// No orbiters in the old room.  Turn off all the lights
				m_pRouter->DispatchMessage(new Message(m_pDevice_This->m_iPK_Device,DEVICEID_DCEROUTER,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_ADJUST_LIGHTS_CONST,2,C_COMMANDPARAMETER_ROOM_CONST,
					StringUtils::itos(m_pDevice_CurrentDetected->m_pRoom->m_PK_Room).c_str(),
					COMMANDPARAMETER_ID_CONST,"0"));
			}
		}
	}

	if( !m_pEntGroupAudioZone_LockedOn && m_bFollowMeMedia && m_pLastEntgroup && m_pPlutoOrbiter->m_pEntGroup && 
		m_pLastEntgroup->m_pWatchingStream )
	{
		MediaStream *pStreamKilled = m_pLastEntgroup->m_pWatchingStream;
		m_pLastEntgroup->StopMedia(pStreamKilled,m_pPlutoOrbiter,true,true);

		map<int,MediaStream *>::iterator itStreams;
		for(itStreams = m_pCore->m_pDataGridDevice->m_mapMediaStream.begin();itStreams != m_pCore->m_pDataGridDevice->m_mapMediaStream.end();++itStreams)
		{
			MediaStream *pStream = (*itStreams).second;
			if( pStream && pStream->m_tStreamParked )
			{
				// Okay,we found a parked stream, now see if there any mobile orbiters in this room for this user
				MobileOrbitersMap::iterator itMO;
				for(itMO=m_pCore->m_pPlutoEvents->m_mapMobileOrbiters.begin();itMO!=m_pCore->m_pPlutoEvents->m_mapMobileOrbiters.end();++itMO)
				{
					DeviceData_Router *pD = (*itMO).second;
					if( pD && pD->m_pMobileOrbiter && pD->m_pMobileOrbiter->m_pPlutoOrbiter && 
						pD->m_pMobileOrbiter->m_pPlutoOrbiter->m_pEntGroup==m_pLastEntgroup && 
						pD->m_pMobileOrbiter->m_pPlutoOrbiter->m_pOCUser==pStream->m_pOCUser &&
						pStream!=pStreamKilled)
					{
						// We found a stream waiting to start here
						pStream->m_PK_DeviceToSendTo = m_pLastEntgroup->m_PK_EntGroup;  // This will need to be more sophisticated for external sources like cd's and sat boxes
						m_pLastEntgroup->StartMedia(pStream,m_pPlutoOrbiter);
					}
				}
			}
		}
	}
*/
}

void MobileOrbiter::NewAssociation(DeviceData_Router *pDevice_PriorDetected)
{
	/*
	PLUTO_SAFETY_LOCK(sSS,m_pCore->m_StreamMutex);  // Be sure we're not running this thing twice at the same time

	// Check for follow me's.  The new media group is okay.
	if( !m_pEntGroupAudioZone_LockedOn && m_bFollowMeMedia && m_pPlutoOrbiter->m_pEntGroup && !m_pPlutoOrbiter->m_pEntGroup->m_pWatchingStream )
	{
		map<int,MediaStream *>::iterator itStreams;
		for(itStreams = m_pCore->m_pDataGridDevice->m_mapMediaStream.begin();itStreams != m_pCore->m_pDataGridDevice->m_mapMediaStream.end();++itStreams)
		{
			MediaStream *pStream = (*itStreams).second;
			// See if this is a stream this user started that is not playing here, or was playing here and is parked
			if( pStream && pStream->m_pOCUser && pStream->m_pOCUser==m_pPlutoOrbiter->m_pOCUser && 
				(pStream->Getm_pEntGroup_Primary()!=m_pPlutoOrbiter->m_pEntGroup ||
				(pStream->Getm_pEntGroup_Primary()==m_pPlutoOrbiter->m_pEntGroup && pStream->m_tStreamParked) ))
			{
				// Got a stream to move
				if( !pStream->m_tStreamParked )
					pStream->Getm_pEntGroup_Primary()->StopMedia(pStream,m_pPlutoOrbiter,true,true);

				pStream->m_PK_DeviceToSendTo = m_pPlutoOrbiter->m_pEntGroup->m_PK_EntGroup;  // This will need to be more sophisticated for external sources like cd's and sat boxes
				m_pPlutoOrbiter->m_pEntGroup->StartMedia(pStream,m_pPlutoOrbiter);
				break;  // We only do 1
			}
		}
	}

	if( !m_pEntGroupAudioZone_LockedOn && m_bFollowMeLights )
	{
		if( m_pDevice_CurrentDetected && m_pDevice_CurrentDetected->m_pRoom )
		{
			m_pRouter->DispatchMessage(new Message(m_pDevice_This->m_iPK_Device,DEVICEID_DCEROUTER,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_ADJUST_LIGHTS_CONST,2,C_COMMANDPARAMETER_ROOM_CONST,
				StringUtils::itos(m_pDevice_CurrentDetected->m_pRoom->m_PK_Room).c_str(),
				COMMANDPARAMETER_ID_CONST,"1"));
		}
	}

	m_pLastEntgroup = m_pPlutoOrbiter->m_pEntGroup;
*/
}

void MobileOrbiter::CreateJoesPinMenu(string &sText,string &sTokens,Message *pMessage)
{
	sText += SimulateKeyPresses;
	sText += "Y~pay~~{c} {c} 1 22 6 993~";
	if( pMessage )
		m_sLastPin=""; // This is called the initial time--not just a repaint

	m_bMaskInput=true;
	// Back to Main Menu
	sText += "N~1~~{c} {c} 1 22 6 990 53 " + m_sLastBasketID + " 5 3~";
	string Amount = m_sLastAmount;
	if( Amount.length()==0 )
	{
		FILE *f = fopen(("/mnt/hlcs2025s/inv" + m_sLastBasketID).c_str(),"rb");
		if( f )
		{
			fseek(f, 0L, SEEK_END);
			int file_size=ftell(f);
			fseek(f, 0L, SEEK_SET);
			char *data_block=(char *)malloc(file_size+1);
			fread(data_block,1,file_size,f);
			data_block[file_size]=0;
			fclose(f);
			string s(data_block);
			delete[] data_block;

			string::size_type pos=0;
			string ID = StringUtils::Tokenize(s,"\t",pos);
			Amount = StringUtils::Tokenize(s,"\t",pos);
		}
		m_sLastAmount=Amount;
	}
	sTokens += "AMT|" + Amount + "|";
}

void MobileOrbiter::CreateJoesInvoiceMenu(string &sText,string &sTokens,Message *pMessage)
{
	sText += SimulateKeyPresses;
	int RowsPerPage=8;

	if( pMessage )
	{
		m_sLastAmount="";
		if( pMessage->m_Parameters[COMMANDPARAMETER_ID_CONST].length()>0 )
			m_sLastBasketID = pMessage->m_Parameters[COMMANDPARAMETER_ID_CONST];
		m_iLastPage = atoi(pMessage->m_Parameters[C_COMMANDPARAMETER_ABS_LEVEL_CONST].c_str());
	}
	FILE *f = fopen(("/mnt/hlcs2025s/inv" + m_sLastBasketID).c_str(),"rb");
	if( f )
	{
		fseek(f, 0L, SEEK_END);
		int file_size=ftell(f);
		fseek(f, 0L, SEEK_SET);
		char *data_block=(char *)malloc(file_size+1);
		fread(data_block,1,file_size,f);
		data_block[file_size]=0;
		fclose(f);
		string s(data_block);
		delete[] data_block;

		string::size_type pos=0;
		string ID = StringUtils::Tokenize(s,"\t",pos);
		string Amount = StringUtils::Tokenize(s,"\t",pos);
		sTokens += "AMT|" + Amount + "|";

		int LineNum=0;
		string Token;
		bool bMore=false;
		while(true)
		{
			string Description = StringUtils::Tokenize(s,"\t",pos);
			string Amount = StringUtils::Tokenize(s,"\t",pos);

			if( Amount.length()==0 && Description.length()==0 )
				break;

			if( LineNum<(RowsPerPage*m_iLastPage-1) )
			{
				LineNum++;
				continue;
			}

			int DisplayLine = LineNum - RowsPerPage*m_iLastPage;
			if( DisplayLine >= RowsPerPage )
			{
				bMore=true;
				break;
			}

			sTokens += "D" + StringUtils::itos(DisplayLine) + "|" + Description + "|";
			sTokens += "A" + StringUtils::itos(DisplayLine) + "|" + Amount + "|";
			LineNum++;
		}

		if( m_iLastPage>0 )
			sText += "U~up~~{c} {c} 1 22 6 990 2 " + StringUtils::itos(m_iLastPage-1) + "~";
		if( bMore )
			sText += "D~down~~{c} {c} 1 22 6 990 2 " + StringUtils::itos(m_iLastPage+1) + "~";

		sText += "Y~pay~~{c} {c} 1 22 6 991~";
	}
}
