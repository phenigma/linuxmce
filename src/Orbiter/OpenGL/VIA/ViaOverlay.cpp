/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "ViaOverlay.h"

#ifdef VIA_DIAG_TEST_UNIT
	#include <stdarg.h>

	#define PLUTO_SAFETY_LOCK(x, y) 

	#define LV_TV	1
	#define LV_WARNING	2
	#define LV_CRITICAL 3

	class LoggerWrapper
	{
		static LoggerWrapper *m_pLoggerWrapper;

	public:
		static LoggerWrapper *GetInstance()
		{
			if(NULL == m_pLoggerWrapper)
				m_pLoggerWrapper = new LoggerWrapper();

			return m_pLoggerWrapper;
		}

		void Write(int iLevel, const char *pcFormat, ... ) 
		{
			va_list argList;
			va_start(argList, pcFormat);
			printf("%s", iLevel == LV_WARNING ? "(WW)\t: " : iLevel == LV_CRITICAL ? "(EE)\t: " : "(II)\t: ");
			printf(pcFormat, argList);
			printf("\n");
			va_end(argList); 
		}
	};

	LoggerWrapper *LoggerWrapper::m_pLoggerWrapper = NULL;
#else
	#include "SDL_Helpers/SDL_Defs.h"
	#include "SDL_Helpers/SDL_pixel.h"
	#include "SDL_Helpers/SDL_SavePNG.h"
	#include "SDL.h"

	using namespace DCE;
#endif
//-------------------------------------------------------------------------------------------------------
ViaOverlay ViaOverlay::m_Instance;
//-------------------------------------------------------------------------------------------------------
ViaOverlay::ViaOverlay() : 
#ifndef VIA_DIAG_TEST_UNIT
	m_ScreenMutex("screen mutex"), 
#endif
	m_lpAlphaSurface(NULL), m_nWidth(0), m_nHeight(0), m_bOverlayInitialized(false),
	m_bHasPopups(false), m_ScreenMask(NULL), m_BufferMask(NULL), m_bOverlaySuspended(false)
{
#ifndef VIA_DIAG_TEST_UNIT
	pthread_mutexattr_t m_ScreenAttr;
	pthread_mutexattr_init(&m_ScreenAttr);
	pthread_mutexattr_settype(&m_ScreenAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	m_ScreenMutex.Init(&m_ScreenAttr);
	pthread_mutexattr_destroy(&m_ScreenAttr); 
#endif

	memset(&m_VMI_Info, 0, sizeof(VMI_INFO_PARAM));
}
//-------------------------------------------------------------------------------------------------------
ViaOverlay::~ViaOverlay()
{
	//restore transparency when orbiter is reloading
	ResetAlphaMask();

	//apply the mask before dying
	WorldChanged();
    
	delete [] m_ScreenMask;
	m_ScreenMask = NULL;

	delete [] m_BufferMask;
	m_BufferMask = NULL;

#ifndef VIA_DIAG_TEST_UNIT
	pthread_mutex_destroy(&m_ScreenMutex.mutex); 
#endif
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WindowCreated(unsigned long nWidth, unsigned long nHeight)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA ViaOverlay::WindowCreated size %d x %d", nWidth, nHeight);

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if(VMI_CreateConnection())
	{
		LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Created connection to VIA drivers!");
		if(CreateAlphaSurface(nWidth, nHeight))
		{
			LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Via overlay activated!");
		}

		//create our local screen mask
		if(NULL == m_ScreenMask)
			m_ScreenMask = new unsigned char [m_nWidth * m_nHeight];

		//the buffer mask
		if(NULL == m_BufferMask)
			m_BufferMask = new unsigned char [m_nWidth * m_nHeight];
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot create connection to VIA drivers!");
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WindowResized()
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA ViaOverlay::WindowResized");

	if(m_bOverlayInitialized)
	{
		if(SetAlphaSurface())
		{
			LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Alpha surface setup done!");
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "#VIA Alpha surface setup failed!");
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WorldChanged()
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 


	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA ViaOverlay::WorldChanged");

	if(m_bOverlayInitialized)
	{
		if(!m_bOverlaySuspended)
			memcpy(m_lpAlphaSurface, m_BufferMask, m_nWidth * m_nHeight);

		if(!m_bHasPopups)
			memcpy(m_ScreenMask, m_BufferMask, m_nWidth * m_nHeight);

		if(UpdateAlphaSurface())
		{
			LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Alpha surface updated!");
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ResetAlphaMask()
{
	if(!m_bOverlayInitialized)
		return;

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Reseting alpha surface...");
	memset(m_BufferMask, 0xFF, m_nWidth * m_nHeight);
	memset(m_ScreenMask, 0xFF, m_nWidth * m_nHeight);

	if(!m_bOverlaySuspended)
	memset(m_lpAlphaSurface, 0xFF, m_nWidth * m_nHeight);

	m_bHasPopups = false;
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::SuspendOverlay()
{
	if(!m_bOverlayInitialized)
		return;

	m_bOverlaySuspended = true;
	memset(m_lpAlphaSurface, 0x00, m_nWidth * m_nHeight);
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ResumeOverlay()
{
	if(!m_bOverlayInitialized)
		return;

	m_bOverlaySuspended = false;
	memcpy(m_lpAlphaSurface, m_BufferMask, m_nWidth * m_nHeight);
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ApplyAlphaMask(int x, int y, int w, int h, const unsigned char *mask)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	if(m_bOverlayInitialized)
		InternalApplyAlphaMask(x, y, w, h, mask);
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::FillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value, bool bMergeToScreen/*=false*/)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	if(m_bOverlayInitialized)
		InternalFillRectangleInAlphaMask(x, y, w, h, value, bMergeToScreen);
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::InternalApplyAlphaMask(int x, int y, int w, int h, const unsigned char *mask)
{
	//clipping for masking

	int offset_x = 0;
	int offset_y = 0;
	int clipped_w = w;
	int clipped_h = h;

	if(x < 0)	
		offset_x = -x;

	if(y < 0)	
		offset_y = -y;

	if(w + x > m_nWidth)
		clipped_w = m_nWidth - x;

	if(h + y > m_nHeight)
		clipped_h = m_nHeight - y;

	if(x + w <= m_nWidth && x >= 0 && y >= 0 && y + h <= m_nHeight && NULL != mask)
	{
		LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Applying alpha for %p (%d,%d,%d,%d) ...", mask, x, y, w, h);

		if(
			x == 0 && y == 0 && w == m_nWidth && h == m_nHeight && 
			offset_x == 0 && offset_y == 0 && clipped_w == w && clipped_h == h
		)
			memcpy(m_BufferMask, mask, w * h);
		else
		{
			if(offset_x < w || offset_y < h)
			{
				for(int i = offset_y; i < clipped_h; i++)
					memcpy(m_BufferMask + (y + i) * m_nWidth + x + offset_x, mask + i * w + offset_x, clipped_w);
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::InternalFillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value, bool bMergeToScreen/*=false*/)
{
	//clip rectangle instead of rejecting it
	if(x < 0)	
		x = 0;

	if(y < 0)	
		y = 0;

	if(w + x > m_nWidth)
		w = m_nWidth - x;

	if(h + y > m_nHeight)
		h = m_nHeight - y;

	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Filled rectangle in alpha mask (%d,%d,%d,%d), value %d...", 
		x, y, w, h, value);

	if(x == 0 && y == 0 && w == m_nWidth && h == m_nHeight)
	{
		memset(m_BufferMask, value, w * h);

		if(bMergeToScreen)
			memset(m_ScreenMask, value, w * h);
	}
	else
	{
		for(int i = 0; i < h; i++)
		{
			memset(m_BufferMask + (y + i) * m_nWidth + x, value, w);

			if(bMergeToScreen)
				memset(m_ScreenMask + (y + i) * m_nWidth + x, value, w);
		}
	}
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::VMI_CreateConnection()
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 
	
	int nReturnCode = VMI_Create(&m_VMI_Info);

	if(nReturnCode != VMI_OK)
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "VMI_Create failed code %d", nReturnCode);

	return VMI_OK == nReturnCode;
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::CreateAlphaSurface(int nWidth, int nHeight)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	DDSURFACEDESC ddSurfaceDesc;
	DDLOCK  ddLock;
	unsigned int dwRet = 0;

	ddSurfaceDesc.dwFourCC = FOURCC_ALPHA;
	ddSurfaceDesc.dwWidth = nWidth;
	ddSurfaceDesc.dwHeight = nHeight;

	dwRet = VMI_DriverProc(&m_VMI_Info, CREATESURFACE, (void *)&ddSurfaceDesc, NULL);
	if (dwRet == VMI_OK)
	{
		LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Created alpha surface for via overlay!");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "#VIA Cannot create alpha surface for via overlay!");
		return false;
	}

	ddLock.dwFourCC = FOURCC_ALPHA;

	dwRet = VMI_DriverProc(&m_VMI_Info, LOCKSURFACE, (void *)&ddLock, NULL);
	if (dwRet == VMI_OK)
	{
		LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Locked alpha surface for via overlay!");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Cannot lock alpha surface for via overlay!");
		return false;
	}

	m_lpAlphaSurface = ddLock.ALPDevice.lpALPOverlaySurface;
	memset(m_lpAlphaSurface, 0xFF, nWidth * nHeight);

	m_bOverlayInitialized = true;
	return true;
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::SetAlphaSurface()
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	ALPHACTRL AlphaCtrl;
	AlphaCtrl.AlphaEnable = TRUE;
	AlphaCtrl.type = ALPHA_STREAM;

	return VMI_OK == VMI_DriverProc(&m_VMI_Info, SETALPHAWIN, (void *)&AlphaCtrl, NULL);
} 
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::UpdateAlphaSurface()
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 

	RECTL rDest;
	rDest.left      = 0;
	rDest.top       = 0;
	rDest.right     = m_nWidth;
	rDest.bottom    = m_nHeight;

	if(VMI_OK != VMI_DriverProc(&m_VMI_Info, UPDATEALPHA, (void *)&rDest, NULL))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "#VIA Failed to update alpha surface!");
		return false;
	}

	return SetAlphaSurface();
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::DumpMask()
{
#ifndef VIA_DIAG_TEST_UNIT
	if(!m_bOverlayInitialized)
		return;

	SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_nWidth, m_nHeight, 32, rmask, gmask, bmask, amask);

	for(int j = 0; j < m_nHeight; j++)
		for(int i = 0; i < m_nWidth; i++)
			putpixel(pSurface, i, j, SDL_MapRGBA(pSurface->format, 128, 128, 128, m_BufferMask[i + j * m_nWidth])); 

	IMG_SavePNG(pSurface, "/home/mask-dump.png");
	SDL_FreeSurface(pSurface);
#endif
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ShowPopup(int x, int y, int w, int h)
{
	if(!m_bOverlayInitialized)
		return;

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 
	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Show popup %d %d %d %d", x, y, w, h);

	InternalFillRectangleInAlphaMask(x, y, w, h, 0x00);

	if(!m_bHasPopups)
	{
		//make sure with have the screen mask up-to-date
		memcpy(m_BufferMask, m_ScreenMask, m_nWidth * m_nHeight);
	}

	m_bHasPopups = true;
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::HidePopup(int x, int y, int w, int h)
{
	if(!m_bOverlayInitialized)
		return;

	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex); 
	LoggerWrapper::GetInstance()->Write(LV_TV, "#VIA Hide popup %d %d %d %d", x, y, w, h);

	memcpy(m_BufferMask, m_ScreenMask, m_nWidth * m_nHeight);
	m_bHasPopups = true;
}
//-------------------------------------------------------------------------------------------------------

