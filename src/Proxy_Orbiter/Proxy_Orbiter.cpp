/*
 xxProxy_Orbiter
 
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
#include "DCE/Logger.h"
#include "Proxy_Orbiter.h"
#include "SerializeClass/ShapesColors.h"
#include "pluto_main/Define_Button.h"
#include "Orbiter/SDL/JpegWrapper.h"

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

#define ADVANCED_OPTIONS_SCREEN "2022"
#define IMAGE_QUALITY_SCREEN    "1274"

//-----------------------------------------------------------------------------------------------------
xxProxy_Orbiter::xxProxy_Orbiter(int DeviceID, int PK_DeviceTemplate, string ServerAddress)
: OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, "", false, 0, 
    0, false), SocketListener("Proxy_Orbiter"), m_ActionMutex( "action" )
{
	m_iImageCounter = 1;
    m_iLastImageSent = -1;

    m_ImageQuality = 70;
	m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on

    //hack
    ServerAddress = "192.168.80.1";
    m_sBaseUrl = "http://" + ServerAddress + "/pluto-admin/";

    pthread_cond_init( &m_ActionCond, NULL );
    m_ActionMutex.Init(NULL, &m_ActionCond);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool xxProxy_Orbiter::GetConfig()
{
    if(!OrbiterSDL::GetConfig())
        return false;

    m_ImageQuality = DATA_Get_ImageQuality();

    if(m_ImageQuality < 10 || m_ImageQuality > 100)
        m_ImageQuality = 70; //default
	m_iTimeoutScreenSaver=0; // disable it
    m_iListenPort = DATA_Get_Listen_Port();
    if( !m_iListenPort )
        m_iListenPort = 3451;

    return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::Run()
{
    m_sRequestUrl = m_sBaseUrl + 
        "index.php?"
        "section=proxySocket&amp;"
        "address=" + m_sMyIPAddress + "&amp;"
        "port=" + StringUtils::ltos(m_iListenPort) + "&amp;"
        "command=XML&amp;";
    m_sPngImageUrl = m_sBaseUrl + "security_images/orbiter_screen.png";

    Initialize(gtSDLGraphic);
    StartListening(m_iListenPort);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ xxProxy_Orbiter::~xxProxy_Orbiter()
{
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
/*virtual*/ void xxProxy_Orbiter::DisplayImageOnScreen(struct SDL_Surface *pScreenImage)
{
	PLUTO_SAFETY_LOCK(sm,m_ScreenMutex);
    if(NULL != m_pScreenHistory_Current)
    {
		g_pPlutoLogger->Write(LV_STATUS, "Proxy_Orbiter::DisplayImageOnScreen Current screen: %s",  m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str());
    }

    SDL_SetAlpha(pScreenImage, SDL_RLEACCEL , SDL_ALPHA_OPAQUE);

    if(pScreenImage->w <= 320 && pScreenImage->h <= 240) //ip phone
    {   
	SDL_SaveBMP(pScreenImage, "screen.bmp");
        //SaveImageToFile(pScreenImage, CURRENT_SCREEN_IMAGE_TEMP);
        string sCmdLine = string("convert -colors 256 ") + "screen.bmp" + " " + CURRENT_SCREEN_IMAGE;
        system(sCmdLine.c_str());
    }
    else
    {
        //generate the jpeg or png image with current screen
        if(m_ImageQuality == 100) //we'll use pngs for best quality
            SaveImageToFile(pScreenImage, CURRENT_SCREEN_IMAGE);
        else
            SDL_SaveJPG(pScreenImage, CURRENT_SCREEN_IMAGE, m_ImageQuality);
    }

    SaveXML(CURRENT_SCREEN_XML);

	m_iImageCounter++;
    
    g_pPlutoLogger->Write(LV_WARNING, "Image/xml generated. Wake up! Screen %s", 
        m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str());
    pthread_cond_broadcast(&m_ActionCond);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::SaveXML(string sFileName)
{
    m_dequeXMLItems.clear();
    GenerateXMLItems(m_pScreenHistory_Current->m_pObj);
    string sSoftKeys = GenerateSoftKeys(m_pScreenHistory_Current->m_pObj);

    string sXMLItems;
    int nCount = 1;
    deque<string>::iterator it;
    for(it = m_dequeXMLItems.begin(); it != m_dequeXMLItems.end(); it++)
    {
        if(nCount > 32)
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "More then 32 objects needed! Screen: %s", 
                m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str());
            break;
        }

        sXMLItems += *it;
        nCount++;
    }

    string sPrompt = "DEBUG: Areas available: " + StringUtils::ltos(nCount - 1) + "/" + 
        StringUtils::ltos(m_dequeXMLItems.size());
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

    FileUtils::WriteBufferIntoFile(CURRENT_SCREEN_XML, const_cast<char *>(sXMLString.c_str()), sXMLString.size());
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::GenerateXMLItems(DesignObj_Orbiter *pObj) //recursive
{
    if(pObj->m_bTabStop)
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
/*virtual*/ string xxProxy_Orbiter::GenerateSoftKeys(DesignObj_Orbiter *pObj)
{
    if(m_sNowPlaying.length() > 0)
        return sAllSoftKeysXml;

    return sNoMediaSoftKeysXml;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::ParseHardKeys()
{
    string::size_type pos = 0;
    string::size_type posTemp = 0;
    string sKeymapping = DATA_Get_Hard_Keys_mapping();
    int nIndex = 0;

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
/*virtual*/ void xxProxy_Orbiter::BeginPaint()
{

}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::EndPaint()
{
	DisplayImageOnScreen(m_pScreenImage);
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::SetImageQuality(unsigned long nImageQuality)
{
    m_ImageQuality = nImageQuality;
    DATA_Set_ImageQuality(nImageQuality, true);

    m_bRerenderScreen = true;
    RedrawObjects();
}
//-----------------------------------------------------------------------------------------------------

bool xxProxy_Orbiter::ReceivedString( Socket *pSocket, string sLine, int nTimeout )
{
    g_pPlutoLogger->Write(LV_WARNING, "Received: %s", sLine.c_str());

	PLUTO_SAFETY_LOCK(am, m_ActionMutex);
	if( sLine.substr(0,5)=="IMAGE" )
	{
        if(m_iLastImageSent == m_iImageCounter)
            pSocket->SendString("IMAGE 0");
        else
        {
            size_t size;
            char *pBuffer = FileUtils::ReadFileIntoBuffer(CURRENT_SCREEN_IMAGE,size);
            if( !pBuffer )
            {
                g_pPlutoLogger->Write(LV_WARNING, "Sent: ERROR");
                pSocket->SendString("ERROR"); // Shouldn't happen
                return true;
            }

            g_pPlutoLogger->Write(LV_WARNING, "Sent: IMAGE %d\\n\\n<IMAGE>", size);
            pSocket->SendString("IMAGE " + StringUtils::itos(size));
            pSocket->SendData(size,pBuffer);
            delete[] pBuffer;

            m_iLastImageSent = m_iImageCounter;
        }
		return true;
	}
    else if( sLine.substr(0,3)=="XML" )
    {
        size_t size;
        char *pBuffer = FileUtils::ReadFileIntoBuffer(CURRENT_SCREEN_XML,size);
        if( !pBuffer )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Sent: ERROR");
            pSocket->SendString("ERROR"); // Shouldn't happen
            return true;
        }

        g_pPlutoLogger->Write(LV_WARNING, "Sent: XML %d\\n<XML>", size);
        pSocket->SendString("XML " + StringUtils::itos(size));
        pSocket->SendData(size,pBuffer);
        delete[] pBuffer;
        return true;
    }
	else if( sLine.substr(0,9)=="PLUTO_KEY" && sLine.size()>10 )
	{
		int Key = atoi( sLine.substr(9).c_str() );
		if( Key )
		{
            if(BUTTON_F4_CONST != Key)
            {
			    ButtonDown(Key);
			    ButtonUp(Key);

                g_pPlutoLogger->Write(LV_WARNING, "Waiting the image/xml to be generated...");        
                timespec abstime;
                abstime.tv_sec = (long) (time(NULL) + 2); //wait max 2 seconds
                abstime.tv_nsec = 0;
                am.TimedCondWait(abstime);

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

        g_pPlutoLogger->Write(LV_WARNING, "Sent: OK");        
		pSocket->SendString("OK");
		return true;
	}
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
	xxProxy_Orbiter *pProxy_Orbiter = new xxProxy_Orbiter(DeviceID, 0, ServerAddress);
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
 
