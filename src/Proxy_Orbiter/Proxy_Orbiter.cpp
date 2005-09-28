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
#include "BD/BDCommandProcessor.h"
#include "VIPShared/BD_CP_ShowImage.h"
#include "VIPShared/BD_CP_SimulateEvent.h"
#include "Proxy_Orbiter_Commands.h"

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
xxProxy_Orbiter::xxProxy_Orbiter(class BDCommandProcessor *pBDCommandProcessor, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress)
: OrbiterSDL(DeviceID, PK_DeviceTemplate, ServerAddress, "", false, 0, 
    0, false), m_pBDCommandProcessor(pBDCommandProcessor)
{
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
    bool bSignalStrengthScreen = false;
    bool bQualityImageScreen = false;
    if(NULL != m_pScreenHistory_Current)
    {
		g_pPlutoLogger->Write(LV_CRITICAL, "Current screen: %s",  m_pScreenHistory_Current->m_pObj->m_ObjectID.c_str());
        bSignalStrengthScreen = m_pScreenHistory_Current->m_pObj->m_ObjectID.find(ADVANCED_OPTIONS_SCREEN) != string::npos;
    }

    if(NULL != m_pScreenHistory_Current)
    {
        bQualityImageScreen = m_pScreenHistory_Current->m_pObj->m_ObjectID.find(IMAGE_QUALITY_SCREEN) != string::npos;
    }

    const string csTempFileName = "TmpScreen.png";

    //generate the jpeg or png image with current screen
    if(m_ImageQuality == 100) //we'll use pngs for best quality
        SaveImageToFile(pScreenImage, csTempFileName);
    else
        SDL_SaveJPG(pScreenImage, csTempFileName.c_str(), m_ImageQuality);

    string sRepeatedKeysList;
    GetRepeatedKeysForScreen(m_pScreenHistory_Current->m_pObj, sRepeatedKeysList);
#ifdef DEBUG
    g_pPlutoLogger->Write(LV_WARNING, "Repeated keys list %s: ", sRepeatedKeysList.c_str());
#endif

    //load the image into a buffer and create 'BD_CP_ShowImage' command
    size_t iImageSize;
    char *pImage = FileUtils::ReadFileIntoBuffer(csTempFileName, iImageSize);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING, "Ready to send a picture, size %d, reporting signal strength %d", iImageSize, bSignalStrengthScreen);
#endif

    BD_CP_ShowImage *pBD_CP_ShowImage = new BD_CP_ShowImage(0, (unsigned long)iImageSize, pImage, 
        (unsigned long)(sRepeatedKeysList.length()), sRepeatedKeysList.c_str(), bSignalStrengthScreen,
        bQualityImageScreen, m_ImageQuality);

    PLUTO_SAFE_DELETE_ARRAY(pImage);

    //finally, send the image
    if( m_pBDCommandProcessor )
        m_pBDCommandProcessor->AddCommand(pBD_CP_ShowImage);

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "ShowImage command added to the queue");
#endif
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::SimulateMouseClick(int x, int y)
{
	//do nothing
#ifndef WIN32
	printf("xxProxy_Orbiter::SimulateMouseClick. The event will be ignored (we don't have mouse on the phone)\n");
#endif	
}
//-----------------------------------------------------------------------------------------------------
/*virtual*/ void xxProxy_Orbiter::SimulateKeyPress(long key)
{
	BD_CP_SimulateEvent *pBD_CP_SimulateEvent = new BD_CP_SimulateEvent(0, key);

	if( m_pBDCommandProcessor )
		m_pBDCommandProcessor->AddCommand(pBD_CP_SimulateEvent);

#ifndef WIN32
	printf("xxProxy_Orbiter::SimulateKeyPress with key code: %d\n", key);
#endif
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
/*virtual*/ void xxProxy_Orbiter::OnReload()
{  
    g_pPlutoLogger->Write(LV_WARNING,"Orbiter reloading");

    //kill the connection. this will kill orbiter and then recreate it when the phone will be detected
    m_pBDCommandProcessor->m_bDead = true;
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
