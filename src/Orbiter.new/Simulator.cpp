#include "Simulator.h"

#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "pluto_main/Define_Button.h"
#include "DCE/Logger.h"

using namespace DCE;

#include "Orbiter.h"

#ifdef WIN32
	#include "Win32/OrbiterWin32Defs.h"
	#include "Win32/MainDialog.h"
	#include "StartOrbiter.h"
#endif

/*

#ifdef WIN32

	#include "Win32/OrbiterWin32Defs.h"
	#include "Win32/MainDialog.h"
	#include "StartOrbiter.h"

	#ifdef WINCE

		#include "stdafx.h"
		#include <commctrl.h>
		#include <sipapi.h>

		#ifdef POCKETFROG
			#include "OrbiterRenderer_PocketFrog.h"
		#else
			#include "OrbiterSDL_WinCE.h"
		#endif
		#ifndef WINCE_x86
			#include <aygshell.h>
		#endif

	#else

		#include <Windows.h>

		#ifdef BLUETOOTH_DONGLE
			#include "SDL_Bluetooth/OrbiterSDLBluetooth.h"
		#elif defined(PROXY_ORBITER)
			#include "Proxy_Orbiter/Proxy_Orbiter.h"
		#else
			#ifdef POCKETFROG
				#include "OrbiterRenderer_PocketFrog.h"
			#else
				#include "OrbiterSDL_Win32.h"
			#endif
		#endif

	#endif

#else //linux stuff
    #ifdef BLUETOOTH_DONGLE
		#include "SDL_Bluetooth/OrbiterSDLBluetooth.h"
    #elif defined(PROXY_ORBITER)
        #include "Proxy_Orbiter/Proxy_Orbiter.h"
	#elif defined(ORBITER_OPENGL)
		#include "OpenGL/Orbiter_OpenGL.h"
	#else //non bluetooth or proxy
		#include "Linux/OrbiterLinux.h"
	#endif
#endif //WIN32

*/

//------------------------------------------------------------------------------------------------------
Simulator *Simulator::m_pInstance = NULL;
//-----------------------------------------------------------------------------------------------------
void *GeneratorThread( void *p)
{
	while(!g_pPlutoLogger)
		Sleep(2000);
	
	g_pPlutoLogger->Write(LV_WARNING, "Simulator enabled");

	Simulator *pSimulator = (Simulator *)p;

	pSimulator->m_bIsRunning = true;

	bool bGenerateMouseClicks = pSimulator->m_bGenerateMouseClicks;
	bool bGenerateKeyboardEvents = pSimulator->m_bGenerateKeyboardEvents;
	bool bOption1 = pSimulator->m_iKeysSetToGenerate == 0;
	bool bOption2 = pSimulator->m_iKeysSetToGenerate == 1;
	bool bOption3 = pSimulator->m_iKeysSetToGenerate == 2;

	int iDelayMin = pSimulator->m_dwDelayMin;
	int iDelayMax = pSimulator->m_dwDelayMax;

	int iTimeout = pSimulator->m_dwTimeout;

	if(iDelayMin == iDelayMax)
		iDelayMax++;

	int iButtonsPerClick = pSimulator->m_iNumberOfButtonsPerClick;

	string HomeScreen = pSimulator->m_sHomeScreen;

	int x, y, delay;
	srand( (unsigned)time(NULL) );
	static int Count = 0;

	
/*#ifdef BLUETOOTH_DONGLE
	Orbiter *pOrbiter = (OrbiterSDLBluetooth *)pSimulator->m_pOrbiter;
#elif defined(PROXY_ORBITER)
	Orbiter *pOrbiter = (Proxy_Orbiter *)pSimulator->m_pOrbiter;
#else
	#ifdef WIN32
<<<<<<< .mine
		#ifdef WINCE

			#ifdef POCKETFROG
				OrbiterRenderer_PocketFrog *pOrbiter = OrbiterRenderer_PocketFrog::GetInstance();
			#else
				OrbiterSDL_WinCE *pOrbiter = OrbiterSDL_WinCE::GetInstance();
			#endif

		#else

			#ifdef POCKETFROG
				OrbiterRenderer_PocketFrog *pOrbiter = OrbiterRenderer_PocketFrog::GetInstance();
			#else
				OrbiterSDL_Win32 *pOrbiter = OrbiterSDL_Win32::GetInstance();
			#endif

		#endif
=======
		Orbiter *pOrbiter = ORBITER_CLASS::GetInstance();
>>>>>>> .r9538
	#else
		Orbiter *pOrbiter = (OrbiterLinux *)pSimulator->m_pOrbiter;
	#endif
#endif */

	Orbiter *pOrbiter = pSimulator->m_pOrbiter;

	while(true)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Simulator: generating new event (orbiter : %p)", pOrbiter);

		if(!pOrbiter || pOrbiter->m_bQuit)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Orbiter is NULL!");
			pSimulator->m_bIsRunning = false;
			return NULL;
		}

		if(pSimulator->m_bStopGeneratorThread)
		{
			pSimulator->m_bIsRunning = false;
			return NULL;
		}

		if(time(NULL) - pOrbiter->GetLastScreenChangedTime() > iTimeout) //1 hour
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "@@@ Got stuck into the screen with id: %s @@@",
				pOrbiter->GetCurrentScreenID().c_str());

			if(!pOrbiter->m_bQuit && HomeScreen != "")
				pOrbiter->GotoDesignObj(HomeScreen);
		}

		delay = iDelayMin + rand() % (iDelayMax - iDelayMin);
		Simulator::SimulateActionDelay(delay);

		if(Count >= iButtonsPerClick || !bGenerateKeyboardEvents)
		{
			Count = 0;

			x = rand() % pOrbiter->m_iImageWidth;
			y = rand() % pOrbiter->m_iImageHeight;

			if(!pOrbiter->m_bQuit && bGenerateMouseClicks)
				pOrbiter->SimulateMouseClick(x, y);
		}
		else
		{
			Count++;
			//generate keys events

			if(bOption1)
			{
				int count = 0;
				long list[128];

				long c;
				for(c = BUTTON_a_CONST; c <= BUTTON_z_CONST; c++)
					list[count++] = c;

				for(c = BUTTON_A_CONST; c <= BUTTON_Z_CONST; c++)
					list[count++] = c;

				for(c = BUTTON_Rept_1_CONST; c <= BUTTON_Rept_0_CONST; c++)
					list[count++] = c;

				int index = rand() % count;

				if(!pOrbiter->m_bQuit)
					pOrbiter->SimulateKeyPress(list[index]);
			}
			else if(bOption2)
			{
				long list[] =
				{
					BUTTON_Up_Arrow_CONST, BUTTON_Down_Arrow_CONST,
					BUTTON_Left_Arrow_CONST, BUTTON_Right_Arrow_CONST, BUTTON_Enter_CONST
				};

				int index = rand() % sizeof(list)/sizeof(list[0]);

				if(!pOrbiter->m_bQuit)
					pOrbiter->SimulateKeyPress(list[index]);
			}
			else if(bOption3) //phone keys
			{
				long list[] =
				{
					BUTTON_1_CONST,
					BUTTON_2_CONST,
					BUTTON_3_CONST,
					BUTTON_4_CONST,
					BUTTON_5_CONST,
					BUTTON_6_CONST,
					BUTTON_7_CONST,
					BUTTON_8_CONST,
					BUTTON_9_CONST,
					BUTTON_0_CONST,
					BUTTON_Phone_C_CONST,
					BUTTON_Phone_Pencil_CONST,
					BUTTON_Phone_Talk_CONST,
					BUTTON_Phone_End_CONST,
					BUTTON_Phone_Soft_left_CONST,
					BUTTON_Phone_Soft_right_CONST,
					BUTTON_Asterisk_CONST,
                    BUTTON_Pound_CONST,
                    BUTTON_Rept_1_CONST,
                    BUTTON_Rept_2_CONST,
                    BUTTON_Rept_3_CONST,
                    BUTTON_Rept_4_CONST,
                    BUTTON_Rept_5_CONST,
                    BUTTON_Rept_6_CONST,
                    BUTTON_Rept_7_CONST,
                    BUTTON_Rept_8_CONST,
                    BUTTON_Rept_9_CONST,
                    BUTTON_Rept_0_CONST,
                    BUTTON_Rept_Phone_C_CONST,
                    BUTTON_Rept_Phone_Pencil_CONST,
                    BUTTON_Rept_Phone_Talk_CONST,
                    BUTTON_Rept_Phone_End_CONST,
                    BUTTON_Rept_Phone_Soft_left_CONST,
					BUTTON_Rept_Phone_Soft_right_CONST,
                    BUTTON_Rept_Asterisk_CONST,
                    BUTTON_Rept_Pound_CONST
				};
				int index = rand() % sizeof(list)/sizeof(list[0]);

				if(!pOrbiter->m_bQuit)
					pOrbiter->SimulateKeyPress(list[index]);
			}
		}
	}

	pSimulator->m_bIsRunning = false;
	return NULL;
}
//------------------------------------------------------------------------------------------------------
Simulator::Simulator()
{
	//defaults
	m_bEnableGenerator = 0;
	m_dwDelayMin = 500;
	m_dwDelayMax = 5000;
	m_dwTimeout = 3600;  // 1 Hour
	m_iNumberOfButtonsPerClick = 0;
	m_bGenerateMouseClicks = 1;
	m_bGenerateKeyboardEvents = 1;
	m_iKeysSetToGenerate = 1;
	m_sHomeScreen = "1255";
	m_bTryToDetermineAutomatically = false;
	m_bFullScreen = false;
	m_bUseOpenGL = false;
	m_bLogToServer = false;

	m_iMilisecondsTransition = 500;
	m_iMilisecondsHighLight = 200;

	m_bStopGeneratorThread = false;
	m_dwStartGeneratorThreadDelay = 0;

	m_bIsRunning = false;
}
//------------------------------------------------------------------------------------------------------
Simulator::~Simulator()
{
    m_bStopGeneratorThread = true;

    time_t tTime = time(NULL);
    while(m_bIsRunning)
    {
        Sleep(10);
        if( tTime + 5 < time(NULL) )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Failed to stop the simulator thread!");
            return;
        }
    }
}
//------------------------------------------------------------------------------------------------------
/*static*/ Simulator* Simulator::GetInstance()
{
	if(NULL == m_pInstance)
		m_pInstance = new Simulator();

	return m_pInstance;
}
//------------------------------------------------------------------------------------------------------
/*static*/ void Simulator::SimulateActionDelay(long delay)
{
	Sleep(delay);
}
//------------------------------------------------------------------------------------------------------
void Simulator::LoadConfigurationFile(string sConfigurationFile)
{
	m_sConfigurationFile = sConfigurationFile;

	vector<string> vectString;
	FileUtils::ReadFileIntoVector( sConfigurationFile, vectString );

	for(vector<string>::iterator it = vectString.begin(); it != vectString.end(); ++it)
	{
		string sItem = *it;
		string::size_type pos_Equal;
		// Skip over any lines that start with # or /, or don't have an =
		if( sItem[0]=='#' || sItem[0]=='/' || (pos_Equal=sItem.find('='))==string::npos )
			continue;

		string::size_type pos_Slash = sItem.find("//");

		string Token=sItem.substr(0,pos_Equal);
		StringUtils::TrimSpaces(Token);
		string Value;
		if( pos_Slash==string::npos )
			Value = sItem.substr(pos_Equal+1);
		else
			Value = sItem.substr(pos_Equal+1,pos_Slash-pos_Equal-1);

		m_mapParameters[Token]=StringUtils::TrimSpaces(Value);
	}

	m_bEnableGenerator = ReadInteger("EnableGenerator", m_bEnableGenerator) != 0;
	m_dwDelayMin = ReadInteger("DelayMin", m_dwDelayMin);
	m_dwDelayMax = ReadInteger("DelayMax", m_dwDelayMin);
	m_dwTimeout = ReadInteger("Timeout", m_dwTimeout);
	m_iNumberOfButtonsPerClick = ReadInteger("NumberOfButtonsPerClick", m_iNumberOfButtonsPerClick);
	m_bGenerateMouseClicks = ReadInteger("GenerateMouseClicks", m_bGenerateMouseClicks) != 0;
	m_bGenerateKeyboardEvents = ReadInteger("GenerateKeyboardEvents", m_bGenerateKeyboardEvents) != 0;
	m_iKeysSetToGenerate = ReadInteger("KeysSetToGenerate", m_iKeysSetToGenerate);
	m_sHomeScreen = ReadString("HomeScreen", m_sHomeScreen);

	m_iMilisecondsTransition = ReadInteger("MilisecondsTransition", m_iMilisecondsTransition);
	m_iMilisecondsHighLight = ReadInteger("MilisecondsHighLight", m_iMilisecondsHighLight);
	

	m_bTryToDetermineAutomatically = false;
	m_bFullScreen = ReadInteger("FullScreen", (int)m_bFullScreen) != 0;
	m_bUseOpenGL = ReadInteger("UseOpenGL", (int)m_bLogToServer) != 0;
	m_bLogToServer = ReadInteger("LogToServer", (int)m_bLogToServer) != 0;
	m_sDeviceID = ReadString("DeviceID", m_sDeviceID);
	m_sRouterIP = ReadString("RouterIP", m_sRouterIP);

	if(m_dwDelayMax <= m_dwDelayMin)
		m_dwDelayMax = m_dwDelayMin + 1;

#ifndef BLUETOOTH_DONGLE
	#ifdef WIN32
		LoadUI_From_ConfigurationData();
	#endif
#endif

/*
	if(m_bEnableGenerator)
	{
		#ifdef WINCE
			StartRandomEventGenerator(20000);
		#else
			StartRandomEventGenerator(5000);
		#endif
	}
*/
}
//------------------------------------------------------------------------------------------------------
void Simulator::SaveConfigurationFile(string sConfigurationFile)
{
    if(sConfigurationFile == "")
        sConfigurationFile = m_sConfigurationFile;

	vector<string> vectString;
	FileUtils::ReadFileIntoVector( sConfigurationFile, vectString );

	m_mapParameters["EnableGenerator"] = StringUtils::ltos(long(m_bEnableGenerator));
	m_mapParameters["DelayMin"] = StringUtils::ltos(m_dwDelayMin);
	m_mapParameters["DelayMax"] = StringUtils::ltos(m_dwDelayMax);
	m_mapParameters["NumberOfButtonsPerClick"] = StringUtils::ltos(m_iNumberOfButtonsPerClick);
	m_mapParameters["GenerateMouseClicks"] = StringUtils::ltos(long(m_bGenerateMouseClicks));
	m_mapParameters["GenerateKeyboardEvents"] = StringUtils::ltos(long(m_bGenerateKeyboardEvents));
	m_mapParameters["KeysSetToGenerate"] = StringUtils::ltos(m_iKeysSetToGenerate);
	m_mapParameters["HomeScreen"] = m_sHomeScreen;

	m_mapParameters["MilisecondsTransition"] = StringUtils::ltos(m_iMilisecondsTransition);
	m_mapParameters["MilisecondsHighLight"] = StringUtils::ltos(m_iMilisecondsHighLight);

	m_mapParameters["TryToDetermineAutomatically"] = StringUtils::ltos(m_bTryToDetermineAutomatically);
	m_mapParameters["FullScreen"] = StringUtils::ltos(m_bFullScreen);
	m_mapParameters["UseOpenGL"] = StringUtils::ltos(m_bUseOpenGL);
	m_mapParameters["LogToServer"] = StringUtils::ltos(m_bLogToServer);

	m_mapParameters["DeviceID"] = m_sDeviceID;
	m_mapParameters["RouterIP"] = m_sRouterIP;

	// Make a copy of the parameters.  We'll go through the existing file and modify any changed values
	map<string,string> mapParameters_Copy = m_mapParameters;

	for(vector<string>::iterator it = vectString.begin(); it != vectString.end(); ++it)
	{
		string sItem = *it;
		string::size_type pos_Equal;
        // Skip over any lines that start with # or /, or don't have an =
		if( sItem[0]=='#' || sItem[0]=='/' || (pos_Equal=sItem.find('='))==string::npos )
            continue;

		string::size_type pos_Slash = sItem.find("//");

		string Token = sItem.substr(0,pos_Equal);
		StringUtils::TrimSpaces(Token);

        string Value;

		if( pos_Slash==string::npos )
			Value = sItem.substr(pos_Equal+1);
		else
			Value = sItem.substr(pos_Equal+1,pos_Slash-pos_Equal-1);

		if( mapParameters_Copy.find(Token)==mapParameters_Copy.end() )
			continue; // This token exists in the file but not our map.  We won't touch it

		if( StringUtils::TrimSpaces(mapParameters_Copy[Token])==StringUtils::TrimSpaces(Value) )
		{
			mapParameters_Copy.erase(Token); // We're taking care of this one here since the token exists
			continue; // The value is the same as what's in the file.  Skip it
		}

		// We need to update what's in the file to reflect this value
		if( pos_Slash==string::npos )
			sItem.replace(pos_Equal+1, sItem.length()-pos_Equal-1, mapParameters_Copy[Token]);
		else // Preserve the // and anything after it
			sItem.replace(pos_Equal+1, pos_Slash-pos_Equal-2, mapParameters_Copy[Token]);

		*it = sItem;

		mapParameters_Copy.erase(Token); // We're taking care of this one here since the token exists
    }

	// Add the new ones
	for(map<string,string>::iterator itm = mapParameters_Copy.begin();itm != mapParameters_Copy.end(); ++itm)
		vectString.push_back((*itm).first + "=" + (*itm).second);

	// Rewrite the config file
	FileUtils::WriteVectorToFile(sConfigurationFile, vectString);
}
//------------------------------------------------------------------------------------------------------
void Simulator::StartRandomEventGenerator()
{
	m_bStopGeneratorThread = false;

	pthread_create(&m_GeneratorThreadID, NULL, GeneratorThread, (void*)this);
	pthread_detach(m_GeneratorThreadID);
}
//------------------------------------------------------------------------------------------------------
void Simulator::StopRandomEventGenerator()
{
	m_bStopGeneratorThread = true;
}
//------------------------------------------------------------------------------------------------------
/*inline*/ int Simulator::ReadInteger(string sToken, int iDefaultValue/* = 0*/)
{
    int iValue = iDefaultValue;

    if( m_mapParameters_Exists(sToken) )
    {
        try
        {
            iValue = atoi(m_mapParameters_Find(sToken).c_str());
        }
        catch(...) {}
    }

    return iValue;
}
//------------------------------------------------------------------------------------------------------
/*inline*/ string Simulator::ReadString(string sToken, string sDefaultValue/* = ""*/)
{
    string sValue = sDefaultValue;

    if( m_mapParameters_Exists(sToken) )
        sValue = m_mapParameters_Find(sToken);

    return sValue;
}
//------------------------------------------------------------------------------------------------------
/*static*/ void Simulator::Cleanup()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}
//------------------------------------------------------------------------------------------------------
int Simulator::GetPreseededInstallValue(string sToken)
{
	string sValue = "0";

	if( m_mapParameters_Exists(sToken) )
		sValue = m_mapParameters_Find(sToken);

	return atoi(sValue.c_str());
}
//------------------------------------------------------------------------------------------------------
bool Simulator::PreseededInstallValueDefined(string sToken)
{
	return m_mapParameters_Exists(sToken);
}
//------------------------------------------------------------------------------------------------------
