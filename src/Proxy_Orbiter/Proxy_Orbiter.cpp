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
xxProxy_Orbiter::xxProxy_Orbiter(int ListenPort, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress)
: OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, "", false, 0, 
    0, false), SocketListener("Proxy_Orbiter")
{
	if( !ListenPort )
		m_iListenPort = 3451;
	else
		m_iListenPort = ListenPort;

	m_iImageCounter = 1;
    m_ImageQuality = 70;
	m_bDisplayOn=true;  // Override the default behavior -- when the phone starts the display is already on
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ bool xxProxy_Orbiter::GetConfig()
{
    if(!OrbiterSDL::GetConfig())
        return false;

    m_ImageQuality = DATA_Get_ImageQuality();

    if(m_ImageQuality < 10 || m_ImageQuality > 100)
        m_ImageQuality = 70; //default

    Initialize(gtSDLGraphic);
	StartListening(m_iListenPort);

    return true;
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ xxProxy_Orbiter::~xxProxy_Orbiter()
{
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

    //generate the jpeg or png image with current screen
    if(m_ImageQuality == 100) //we'll use pngs for best quality
        SaveImageToFile(pScreenImage, CURRENT_SCREEN);
    else
        SDL_SaveJPG(pScreenImage, CURRENT_SCREEN, m_ImageQuality);

	m_iImageCounter++;
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
	PLUTO_SAFETY_LOCK(sm,m_ScreenMutex);
	if( sLine.substr(0,5)=="IMAGE" )
	{
		int ConnectionID = 0;
		if( sLine.size() > 7 )
			ConnectionID = atoi( sLine.substr(6).c_str() );
		if( !ConnectionID )
			ConnectionID = pSocket->m_iSocketCounter;

		if( m_mapID_ImageCounter[ConnectionID]==m_iImageCounter )
		{
			pSocket->SendString("IMAGE 0"); // No new image
			return true;
		}
		m_mapID_ImageCounter[ConnectionID]=m_iImageCounter;

		size_t size;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(CURRENT_SCREEN,size);
		if( !pBuffer )
		{
			pSocket->SendString("ERROR"); // Shouldn't happen
			return true;
		}

		pSocket->SendString("IMAGE " + StringUtils::itos(size));
		pSocket->SendData(size,pBuffer);
		delete[] pBuffer;
		return true;
	}
	else if( sLine.substr(0,9)=="PLUTO_KEY" && sLine.size()>10 )
	{
		int Key = atoi( sLine.substr(9).c_str() );
		if( Key )
		{
			ButtonDown(Key);
			ButtonUp(Key);
			pSocket->SendString("OK");
		}
		else
			pSocket->SendString("ERROR"); // Shouldn't happen
		return true;
	}
	else if( sLine.substr(0,5)=="TOUCH" && sLine.size()>6 )
	{
		int X = atoi(sLine.substr(6).c_str()),Y = 0;
		string::size_type pos_y = sLine.find('x');
		if( pos_y!=string::npos )
			Y = atoi(sLine.substr(pos_y+1).c_str());
		RegionDown(X,Y);
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
	xxProxy_Orbiter *pProxy_Orbiter = new xxProxy_Orbiter(NULL,DeviceID, 0, ServerAddress);
	if ( pProxy_Orbiter->GetConfig() && pProxy_Orbiter->Connect(pProxy_Orbiter->PK_DeviceTemplate_get()) ) 
	{
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
 