/*
 Proxy_Orbiter
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to 
 the terms of the Pluto Public License, available at: 
 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 
 See the Pluto Public License for more details.
 
 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/HttpUtils.h"
#include "DCE/Logger.h"
#include "Proxy_Orbiter.h"
#include "SerializeClass/ShapesColors.h"
#include "pluto_main/Define_Button.h"
#include "Orbiter/SDL/JpegWrapper.h"
#include "Orbiter/ScreenHistory.h"
#include "pluto_main/Define_DesignObj.h"

#include "DataGrid.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_video.h>
#include <SDL_rwops.h>

#include "sge.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "png.h"

using namespace DCE;
#if defined(WIN32) 
void WriteStatusOutput(const char *) {} //do nothing
#endif

#define RELEVANT_SCREEN_INTERVAL 800
#define CISCO_LISTEN_PORT_START 3451
#define REQUEST_INTERVAL_TIMEOUT 400
#define CISCO_FAILURE_WATCHDOG_INTERVAL 1000
#define MINIMUM_PUSH_INTERVAL 6000

//-----------------------------------------------------------------------------------------------------
Proxy_Orbiter::Proxy_Orbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress)
: OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, "", false, 0, 
    0, false), SocketListener("Proxy_Orbiter"), m_ActionMutex("action"), m_ResourcesMutex("resources")
{
	m_iImageCounter = 1;
    m_iLastImageSent = -1;
	m_nCurrentScreenId = 0;

    m_ImageQuality = 70;
	m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on

    m_sInternalServerAddress = "192.168.80.1";
    m_sBaseUrl = "http://" + m_sInternalServerAddress + "/pluto-admin/";

    pthread_cond_init( &m_ActionCond, NULL );
    m_ActionMutex.Init(NULL, &m_ActionCond);
	m_ResourcesMutex.Init(NULL);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool Proxy_Orbiter::GetConfig()
{
    if(!OrbiterSDL::GetConfig())
        return false;

    m_ImageQuality = DATA_Get_ImageQuality();
	m_sRemotePhoneIP = DATA_Get_Remote_Phone_IP();

    if(m_ImageQuality < 10 || m_ImageQuality > 100)
        m_ImageQuality = 70; //default
	m_iTimeoutScreenSaver=0; // disable it
    m_iListenPort = DATA_Get_Listen_Port();
    if( !m_iListenPort )
        m_iListenPort = CISCO_LISTEN_PORT_START;

    return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::Run()
{
    m_sRequestUrl = m_sBaseUrl + 
        "index.php?"
        "section=proxySocket&amp;"
        "address=" + m_sInternalServerAddress + "&amp;"
        "port=" + StringUtils::ltos(m_iListenPort) + "&amp;"
        "command=XML&amp;"
		"deviceID=" + StringUtils::ltos(m_dwPK_Device) + "&amp;";
	m_sPngImageUrl = m_sBaseUrl + "security_images/" + StringUtils::ltos(m_dwPK_Device) + "_cisco.png";

    Initialize(gtSDLGraphic);
    StartListening(m_iListenPort);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ Proxy_Orbiter::~Proxy_Orbiter()
{
	Sleep(2500);
	PushRefreshEvent(true, true); //force push, ignore any previous push 
    pthread_mutex_destroy(&m_ActionMutex.mutex);
}
//-----------------------------------------------------------------------------------------------------
void SaveImageToFile(struct SDL_Surface *pScreenImage, string FileName)
{
    SDL_Surface * Drawing = pScreenImage;

    png_bytepp image_rows;
    FILE * File;
    int BitsPerColor;

    File = fopen(FileName.c_str(), "wb");

    image_rows = new png_bytep[Drawing->h];
    for (int n = 0; n < Drawing->h; n++)
        image_rows[n] = (unsigned char *) Drawing->pixels + n * Drawing->pitch;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop png_info = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, File);
    png_set_filter(png_ptr, 0, PNG_FILTER_NONE);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    BitsPerColor = Drawing->format->BitsPerPixel / Drawing->format->BytesPerPixel;
    png_set_IHDR(png_ptr, png_info, Drawing->w, Drawing->h, BitsPerColor, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, png_info);
    png_write_image(png_ptr, image_rows);
    png_write_end(png_ptr, png_info);

    delete [] image_rows;

    png_destroy_write_struct(&png_ptr, &png_info);
    fclose(File);
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::RealRedraw( void *data )
{
	m_dequeCellXMLItems.clear();
	m_bRerenderScreen = true; //force full redraw

	Orbiter::RealRedraw(data);
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::RedrawObjects()
{
	PLUTO_SAFETY_LOCK(rm,m_NeedRedrawVarMutex);
	if(m_vectObjs_NeedRedraw.size() == 0 && m_vectTexts_NeedRedraw.size() == 0 && m_bRerenderScreen==false)
		return; // Nothing to do anyway

	CallMaintenanceInMiliseconds( 0, (OrbiterCallBack)&Proxy_Orbiter::RealRedraw, NULL, pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
string Proxy_Orbiter::GetDevicePngFileName()
{
	return StringUtils::ltos(m_dwPK_Device) + "_" + CURRENT_SCREEN_IMAGE;
}
//-----------------------------------------------------------------------------------------------------
string Proxy_Orbiter::GetDeviceXmlFileName()
{
	return StringUtils::ltos(m_dwPK_Device) + "_" + CURRENT_SCREEN_XML;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::DisplayImageOnScreen(struct SDL_Surface *pScreenImage)
{
	PLUTO_SAFETY_LOCK(sm,m_ScreenMutex);

	if(NULL != m_pScreenHistory_Current)
    {
		g_pPlutoLogger->Write(LV_STATUS, "Proxy_Orbiter::DisplayImageOnScreen Current screen: %s",  m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
    }

    if(pScreenImage->w <= 320 && pScreenImage->h <= 240) //ip phone
		m_ImageQuality = 100;

	string sDevicePng = GetDevicePngFileName();
	string sDeviceXml = GetDeviceXmlFileName();

	PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
	//generate the jpeg or png image with current screen
    if(m_ImageQuality == 100) //we'll use pngs for best quality
        SaveImageToFile(pScreenImage, sDevicePng);
    else
        SDL_SaveJPG(pScreenImage, sDevicePng.c_str(), m_ImageQuality);

    SaveXML(sDeviceXml);
	rm.Release();

	m_nCurrentScreenId = m_pScreenHistory_Current->GetObj()->m_iBaseObjectID;
	g_pPlutoLogger->Write(LV_STATUS, "Current screen generated: %d", m_nCurrentScreenId);
	
	PLUTO_SAFETY_LOCK(am, m_ActionMutex);
    g_pPlutoLogger->Write(LV_STATUS, "Image/xml generated. Wake up! Screen %s", 
        m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
    pthread_cond_broadcast(&m_ActionCond);

	if(m_iListenPort >= CISCO_LISTEN_PORT_START && m_iListenPort < CISCO_LISTEN_PORT_START + 10 && //only cisco orbiters
		!IsProcessingRequest() 
	)
	{
		if(!PendingCallbackScheduled((OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventTask))
		{
			g_pPlutoLogger->Write(LV_STATUS, "Scheduling push refresh event to execute in 1000 ms");
			CallMaintenanceInMiliseconds(1000, (OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventTask, NULL, pe_ALL);
		}
	}
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_Orbiter::PushRefreshEvent(bool bForce,bool bIgnoreMinimumInterval/*=false*/)
{
    static bool bFirstTime = true;
    static timespec tLastImageGenerated;
    timespec tCurrentImageGenerated;
    gettimeofday(&tCurrentImageGenerated, NULL);
    timespec tInterval = tCurrentImageGenerated - tLastImageGenerated;
	long nMilisecondsPassed = tInterval.tv_sec * 1000 + tInterval.tv_nsec / 1000000;
	
	g_pPlutoLogger->Write(LV_STATUS, "Time for last push event %d ms", nMilisecondsPassed);
	g_pPlutoLogger->Write(LV_STATUS, "Current screen id %d", m_nCurrentScreenId);

	if(m_nCurrentScreenId == DESIGNOBJ_mnuDVDmenu_CONST)
		bIgnoreMinimumInterval = false;

	if(!bIgnoreMinimumInterval)
		g_pPlutoLogger->Write(LV_STATUS, "Minimum interval NOT ignored!");		
	
	if(!bIgnoreMinimumInterval && !bFirstTime && nMilisecondsPassed < MINIMUM_PUSH_INTERVAL && !bForce)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Ignoring push event request...");
		return false;
	}

	tLastImageGenerated = tCurrentImageGenerated;	
    bFirstTime = false;

	g_pPlutoLogger->Write(LV_WARNING, "Need to refresh phone's browser!");

	m_bPhoneRespondedToPush = false;
    vector<string> vectHeaders;
    map<string, string> mapParams;
	string sPriorityLevel = bForce ? "0" : "1";
    string sRequestUrl = 
            string() + 
            "<CiscoIPPhoneExecute>"
                "<ExecuteItem Priority=\"" + sPriorityLevel + "\" URL=\"" + m_sRequestUrl + "\"/>"
            "</CiscoIPPhoneExecute>";
    mapParams["XML"] = StringUtils::URLEncode(sRequestUrl);

    string Response = HttpPost("http://" + m_sRemotePhoneIP + "/CGI/Execute", vectHeaders, mapParams, 
            "user", "pluto");

    g_pPlutoLogger->Write(LV_STATUS, "XML param req %s", sRequestUrl.c_str());
    g_pPlutoLogger->Write(LV_WARNING, "Push phone action completed with response: %s", Response.c_str());

	if(!bForce)
	{
		//we have to be sure the cisco phone refreshed; we'll start a watchdog
		CallMaintenanceInMiliseconds(CISCO_FAILURE_WATCHDOG_INTERVAL, (OrbiterCallBack)&Proxy_Orbiter::PushRefreshEventWatchdog, NULL, pe_ALL);
	}

	return Response == "Success";
}
//-----------------------------------------------------------------------------------------------------
void *Proxy_Orbiter::PushRefreshEventWatchdog(void *)
{
	if(!m_bPhoneRespondedToPush)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Cisco phone failed to execute request! Forcing a push...");
		//I sent a push event 1 second ago with priority 1; the phone ignored me
		//It should queue my url request and execute it when to phone goes idle
		//geeeeez, cisco, fix your phone! :(
		PushRefreshEvent(true);
	}

	return NULL;
}
//-----------------------------------------------------------------------------------------------------
bool Proxy_Orbiter::IsProcessingRequest()
{
	g_pPlutoLogger->Write(LV_STATUS, "Is processing request? R: %s", m_bProcessingRequest ? "YES" : "NO");
	return m_bProcessingRequest;
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::StartProcessingRequest()
{
	g_pPlutoLogger->Write(LV_STATUS, "Starting processing request...");
	m_bProcessingRequest = true;
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::EndProcessingRequest()
{
    g_pPlutoLogger->Write(LV_STATUS, "Stopping processing request in few ms...");
	CallMaintenanceInMiliseconds(REQUEST_INTERVAL_TIMEOUT, (OrbiterCallBack)&Proxy_Orbiter::StopProcessingRequest, this, pe_ALL );
}
//-----------------------------------------------------------------------------------------------------
void Proxy_Orbiter::StopProcessingRequest()
{
    g_pPlutoLogger->Write(LV_STATUS, "Stopped processing request...");
	m_bProcessingRequest = false;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::SaveXML(string sFileName)
{
    m_dequeXMLItems.clear();
    GenerateXMLItems(m_pScreenHistory_Current->GetObj());
    string sSoftKeys = GenerateSoftKeys(m_pScreenHistory_Current->GetObj());

    string sXMLItems;
    int nCount = 1;
    deque<string>::iterator it;
    for(it = m_dequeCellXMLItems.begin(); it != m_dequeCellXMLItems.end(); it++)
    {
        if(nCount > 32)
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "More then 32 objects needed! Screen: %s", 
                m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
            break;
        }

        sXMLItems += *it;
        nCount++;
    }

    for(it = m_dequeXMLItems.begin(); it != m_dequeXMLItems.end(); it++)
    {
        if(nCount > 32)
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "More then 32 objects needed! Screen: %s", 
                m_pScreenHistory_Current->GetObj()->m_ObjectID.c_str());
            break;
        }

        sXMLItems += *it;
        nCount++;
    }

    string sPrompt;
    string sXMLString = 
        "<CiscoIPPhoneGraphicFileMenu>\r\n"
        "<Title>Pluto Orbiter</Title>\r\n"
        "<Prompt>" + sPrompt + "</Prompt>\r\n"
        "<LocationX>0</LocationX>\r\n"
        "<LocationY>0</LocationY>\r\n"
        "<URL>" + m_sPngImageUrl + "</URL>\r\n" + 
        sXMLItems + 
        sSoftKeys + 
        "</CiscoIPPhoneGraphicFileMenu>\r\n";

    FileUtils::WriteBufferIntoFile(GetDeviceXmlFileName(), sXMLString.c_str(), sXMLString.size());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::GenerateXMLItems(DesignObj_Orbiter *pObj) //recursive
{
    bool bTouchArea = false;
    DesignObjZoneList::iterator iZone;
    for( iZone=pObj->m_ZoneList.begin(  );iZone!=pObj->m_ZoneList.end(  );++iZone )
    {
        DesignObjZone *pDesignObjZone = ( *iZone );
        //ExecuteCommandsInList( &pDesignObjZone->m_Commands, pObj, -1, -1, pObj->m_iRepeatParm+1 );

        if(pDesignObjZone->m_Commands.size())
        {
            bTouchArea = true;
            break;
        }
    }

    if(bTouchArea)
    {
        string sX1 = StringUtils::ltos(pObj->m_rPosition.X);
        string sY1 = StringUtils::ltos(pObj->m_rPosition.Y);
        string sX2 = StringUtils::ltos(pObj->m_rPosition.Right());
        string sY2 = StringUtils::ltos(pObj->m_rPosition.Bottom());

        string sTouchX = StringUtils::ltos(pObj->m_rPosition.X + pObj->m_rPosition.Width / 2);
        string sTouchY = StringUtils::ltos(pObj->m_rPosition.Y + pObj->m_rPosition.Height / 2);

        string sXMLItem = 
            "<MenuItem>\r\n"
            "\t<Name>Button</Name>\r\n"
            "\t<URL>" + m_sRequestUrl + "x=" + sTouchX + "&amp;" + "y=" + sTouchY + "</URL>\r\n"
            "\t<TouchArea X1=\"" + sX1 + "\" Y1=\"" + sY1 + "\" X2=\"" + sX2 + "\" Y2=\"" + sY2 + "\"/>\r\n"
            "</MenuItem>\r\n";
        m_dequeXMLItems.push_front(sXMLItem);
    }

    DesignObj_DataList::iterator it;
    for(it = pObj->m_ChildObjects.begin(); it != pObj->m_ChildObjects.end(); ++it)
        GenerateXMLItems((DesignObj_Orbiter *)*it);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ string Proxy_Orbiter::GenerateSoftKeys(DesignObj_Orbiter *pObj)
{
    if(m_sNowPlaying.length() > 0)
        return sAllSoftKeysXml;

    return sNoMediaSoftKeysXml;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::ParseHardKeys()
{
    string::size_type pos = 0;
    string::size_type posTemp = 0;
    string sKeymapping = DATA_Get_Hard_Keys_mapping();
    int nIndex = 1;

    while(pos < DATA_Get_Hard_Keys_mapping().size())
    {
        posTemp = 0;

        //a line
        string sLineToken = StringUtils::Tokenize(sKeymapping, "\n", pos);

        //soft key name
        string sName = StringUtils::Tokenize(sLineToken, "\t", posTemp);

        //key
        int nKey = atoi(StringUtils::Tokenize(sLineToken, "\t", posTemp).c_str());
        if(!nKey)
            continue;

        //media playing flag
        bool bMediaPlaying = 1 == atoi(StringUtils::Tokenize(sLineToken, "\t", posTemp).c_str());
        
        //the message to send
        string sMessageArguments = StringUtils::Tokenize(sLineToken, "\t", posTemp);
        sMessageArguments = StringUtils::Replace(sMessageArguments, "devid", StringUtils::ltos(m_dwPK_Device));
        Message *pMessage = new Message(sMessageArguments);
        if( pMessage->m_dwPK_Device_To<0 )
            pMessage->m_dwPK_Device_To = TranslateVirtualDevice(pMessage->m_dwPK_Device_To);
        m_mapHardKeys[nKey] = pMessage;

        //create xml tokens
        string sXmlSoftKey;

        string sUrl = 
            nKey != BUTTON_F4_CONST ?
            (m_sRequestUrl + "key=" + StringUtils::ltos(nKey)) :
            (m_sBaseUrl + "ServicesMenu.php");

        sXmlSoftKey = 
            "<SoftKeyItem>\r\n"
            "\t<Name>" + sName + "</Name>\r\n"
            "\t<URL>" + sUrl + "</URL>\r\n"
            "\t<Position>" + StringUtils::ltos(nIndex++) + "</Position>\r\n"
            "</SoftKeyItem>\r\n";

        if(!bMediaPlaying)
            sNoMediaSoftKeysXml += sXmlSoftKey;

        sAllSoftKeysXml += sXmlSoftKey;
    }
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::BeginPaint()
{
	m_dequeCellXMLItems.clear();
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::EndPaint()
{
	DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void Proxy_Orbiter::SetImageQuality(unsigned long nImageQuality)
{
    m_ImageQuality = nImageQuality;
    DATA_Set_ImageQuality(nImageQuality, true);

    m_bRerenderScreen = true;
    RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------

bool Proxy_Orbiter::RenderCell( class DesignObj_DataGrid *pObj,  class DataGridTable *pT,  class DataGridCell *pCell,  int j,  int i,  int GraphicToDisplay, PlutoPoint point )
{
    bool bRetValue = OrbiterSDL::RenderCell(pObj, pT, pCell, j, i, GraphicToDisplay, point);

    int x, y, w, h;
    GetGridCellDimensions( pObj,  pCell->m_Colspan,  pCell->m_Rowspan,  j,  i,  x,  y,  w,  h );

    string sX1 = StringUtils::ltos(x);
    string sY1 = StringUtils::ltos(y);
    string sX2 = StringUtils::ltos(x + w);
    string sY2 = StringUtils::ltos(y + h);

    string sTouchX = StringUtils::ltos(x + w / 2);
    string sTouchY = StringUtils::ltos(y + h / 2);

    string sXMLItem = 
        "<MenuItem>\r\n"
        "\t<Name>Button</Name>\r\n"
        "\t<URL>" + m_sRequestUrl + "x=" + sTouchX + "&amp;" + "y=" + sTouchY + "</URL>\r\n"
        "\t<TouchArea X1=\"" + sX1 + "\" Y1=\"" + sY1 + "\" X2=\"" + sX2 + "\" Y2=\"" + sY2 + "\"/>\r\n"
        "</MenuItem>\r\n";

    m_dequeCellXMLItems.push_front(sXMLItem);
    return bRetValue;
}

void Proxy_Orbiter::RenderScreen()
{
    m_dequeCellXMLItems.clear();
    return OrbiterSDL::RenderScreen();
}

bool Proxy_Orbiter::ReceivedString( Socket *pSocket, string sLine, int nTimeout )
{
	if(m_bQuit || m_bReload)
	{
		pSocket->SendString("NOT_CONNECTED");
		return false;
	}

    g_pPlutoLogger->Write(LV_WARNING, "Received: %s", sLine.c_str());
	m_bPhoneRespondedToPush = true;

	PLUTO_SAFETY_LOCK(am, m_ActionMutex);
	
	if( sLine.substr(0,5)=="IMAGE" )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Current screen to analyze: %d", m_nCurrentScreenId);
		if(
				m_nCurrentScreenId == DESIGNOBJ_mnuFilelist_Movies_Video_Music_CONST || 
				m_nCurrentScreenId == DESIGNOBJ_mnuSecurityPanelSmallUI_CONST ||
				m_nCurrentScreenId == DESIGNOBJ_mnuFileSaveSmallUI_CONST
		)
		{
			g_pPlutoLogger->Write(LV_STATUS, "One of those screens. Sleeping 500 ms...");
			Sleep(500);
		}
		
		PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
        
		size_t size;
        char *pBuffer = FileUtils::ReadFileIntoBuffer(GetDevicePngFileName(),size);
        if( !pBuffer )
        {
			g_pPlutoLogger->Write(LV_WARNING, "Sent: ERROR");
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

        g_pPlutoLogger->Write(LV_WARNING, "Sent: IMAGE %d\\n\\n<IMAGE>", size);
        pSocket->SendString("IMAGE " + StringUtils::itos(int(size)));
        pSocket->SendData(int(size),pBuffer);
        delete[] pBuffer;

		return true;
	}
    else if( sLine.substr(0,3)=="XML" )
    {
		PLUTO_SAFETY_LOCK(rm, m_ResourcesMutex);
		
        size_t size;
        char *pBuffer = FileUtils::ReadFileIntoBuffer(GetDeviceXmlFileName(),size);
        if( !pBuffer )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Sent: ERROR");
            pSocket->SendString("ERROR"); // Shouldn't happen
            return true;
        }

        g_pPlutoLogger->Write(LV_WARNING, "Sent: XML %d\\n<XML>", size);
        pSocket->SendString("XML " + StringUtils::itos(int(size)));
        pSocket->SendData(int(size), pBuffer);
        delete[] pBuffer;

		EndProcessingRequest();
		
        return true;
    }
	else if( sLine.substr(0,9)=="PLUTO_KEY" && sLine.size()>10 )
	{
		int Key = atoi( sLine.substr(9).c_str() );
		if( Key )
		{
            if(BUTTON_F4_CONST != Key)
            {
				StartProcessingRequest();
				
			    ButtonDown(Key);
			    ButtonUp(Key);

                g_pPlutoLogger->Write(LV_WARNING, "Waiting the image/xml to be generated...");        
                timespec abstime;
                abstime.tv_sec = (long) (time(NULL) + 2); //wait max 2 seconds
                abstime.tv_nsec = 0;
                am.TimedCondWait(abstime);

		        if(m_bQuit || m_bReload)
        		{
            		pSocket->SendString("NOT_CONNECTED");
            		return false;
        		}

                g_pPlutoLogger->Write(LV_WARNING, "Sent: OK");
			    pSocket->SendString("OK");
            }
            else
            {
                g_pPlutoLogger->Write(LV_WARNING, "F4 was sent. No need to kill proxy orbiter."); 
            }
        }
		else
        {
            g_pPlutoLogger->Write(LV_WARNING, "Sent: ERROR");        
			pSocket->SendString("ERROR"); // Shouldn't happen
        }
		return true;
	}
	else if( sLine.substr(0,5)=="TOUCH" && sLine.size()>6 )
	{
		StartProcessingRequest();
		
		int X = atoi(sLine.substr(6).c_str()),Y = 0;
		string::size_type pos_y = sLine.find('x');
		if( pos_y!=string::npos )
			Y = atoi(sLine.substr(pos_y+1).c_str());
		RegionDown(X,Y);

        g_pPlutoLogger->Write(LV_WARNING, "Waiting the image/xml to be generated...");        
        timespec abstime;
        abstime.tv_sec = (long) (time(NULL) + 2); //wait max 2 seconds
        abstime.tv_nsec = 0;
        am.TimedCondWait(abstime);

		if(m_bQuit || m_bReload)
		{
			pSocket->SendString("NOT_CONNECTED");
			return false;
		}

        g_pPlutoLogger->Write(LV_WARNING, "Sent: OK");        
		pSocket->SendString("OK");
		return true;
	}
	return false;
}

void *Proxy_Orbiter::PushRefreshEventTask(void *)
{
	PushRefreshEvent(false, true);
	return NULL;
}

void Proxy_Orbiter::CMD_Quit(string &sCMD_Result,Message *pMessage)
{
	m_bQuit = true;
	pthread_cond_broadcast(&m_ActionCond);
}

void Proxy_Orbiter::CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage)
{
	m_bQuit = true;
	pthread_cond_broadcast(&m_ActionCond);
}

void Proxy_Orbiter::CMD_Regen_Screen(string &sCMD_Result,Message *pMessage)
{
	Orbiter::CMD_Regen_Screen(sCMD_Result, pMessage);
	m_bQuit = true;
	pthread_cond_broadcast(&m_ActionCond);
}
	
void Proxy_Orbiter::OnReload()
{
	m_bReload = true;
	m_bQuit = true;
	
	Orbiter_Command::OnReload(); 
	pthread_cond_broadcast(&m_ActionCond);
}

bool Proxy_Orbiter::IsRepeatedKeyForScreen(DesignObj_Orbiter* pObj, int iPK_Button)
{
	//we don't have a key up and a key down event for web orbiter or small ui
	//using repeated keys can be dangerous, since the command attached to the object
	//with repeated key enabled will be send forever.

	//no repeated keys for small ui or web orbiter
	return false;
}

void LoadUI_From_ConfigurationData()
{
}

extern void (*g_pDeadlockHandler)(PlutoLock *pPlutoLock);
extern void (*g_pSocketCrashHandler)(Socket *pSocket);
extern Command_Impl *g_pCommand_Impl;
void DeadlockHandler(PlutoLock *pPlutoLock);
void SocketCrashHandler(Socket *pSocket);

bool StartOrbiter(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress)
{
	Proxy_Orbiter *pProxy_Orbiter = new Proxy_Orbiter(DeviceID, 0, ServerAddress);
	if ( pProxy_Orbiter->GetConfig() && pProxy_Orbiter->Connect(pProxy_Orbiter->PK_DeviceTemplate_get()) ) 
	{
        pProxy_Orbiter->Run();
		g_pCommand_Impl=pProxy_Orbiter;
		g_pDeadlockHandler=DeadlockHandler;
		g_pSocketCrashHandler=SocketCrashHandler;
		g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
		pProxy_Orbiter->CreateChildren();
		pthread_join(pProxy_Orbiter->m_RequestHandlerThread, NULL);  // This function will return when the device is shutting down
		g_pDeadlockHandler=NULL;
		g_pSocketCrashHandler=NULL;
	} 
	else 
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Connect() Failed");
	}

	bool bReload = pProxy_Orbiter->m_bReload;
	delete pProxy_Orbiter;
	return bReload;
}
 
