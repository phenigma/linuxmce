#include "ViaOverlay.h"

#include "DCE/Logger.h"
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
ViaOverlay ViaOverlay::m_Instance;
//-------------------------------------------------------------------------------------------------------
ViaOverlay::ViaOverlay() : m_lpAlphaSurface(NULL), m_ulWidth(0), m_ulHeight(0), m_bOverlayInitialized(false)
{
	memset(&m_VMI_Info, 0, sizeof(VMI_INFO_PARAM));
}
//-------------------------------------------------------------------------------------------------------
ViaOverlay::~ViaOverlay()
{

}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WindowCreated(unsigned long ulWidth, unsigned long ulHeight)
{
	g_pPlutoLogger->Write(LV_STATUS, "#VIA ViaOverlay::WindowCreated");

	m_ulWidth = ulWidth;
	m_ulHeight = ulHeight;

	if(VMI_CreateConnection())
	{
		g_pPlutoLogger->Write(LV_WARNING, "#VIA Created connection to VIA drivers!");
		if(CreateAlphaSurface(ulWidth, ulHeight))
		{
			g_pPlutoLogger->Write(LV_WARNING, "#VIA Via overlay activated!");
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot create connection to VIA drivers!");
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WindowResized()
{
	g_pPlutoLogger->Write(LV_STATUS, "#VIA ViaOverlay::WindowResized");

	if(m_bOverlayInitialized)
	{
		if(SetAlphaSurface())
		{
			g_pPlutoLogger->Write(LV_WARNING, "#VIA Alpha surface setup done!");
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "#VIA Alpha surface setup failed!");
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WorldChanged()
{
	g_pPlutoLogger->Write(LV_STATUS, "#VIA ViaOverlay::WorldChanged");

	if(m_bOverlayInitialized)
	{
		Test_FillAlphaSurface();

		if(UpdateAlphaSurface())
		{
			g_pPlutoLogger->Write(LV_WARNING, "#VIA Alpha surface updated!");
		}
	}
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::VMI_CreateConnection()
{
	return VMI_OK == VMI_Create(&m_VMI_Info);
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::CreateAlphaSurface(unsigned long ulWidth, unsigned long ulHeight)
{
	DDSURFACEDESC ddSurfaceDesc;
	DDLOCK  ddLock;
	unsigned int dwRet = 0;

	ddSurfaceDesc.dwFourCC = FOURCC_ALPHA;
	ddSurfaceDesc.dwWidth = ulWidth;
	ddSurfaceDesc.dwHeight = ulHeight;

	dwRet = VMI_DriverProc(&m_VMI_Info, CREATESURFACE, (void *)&ddSurfaceDesc, NULL);
	if (dwRet == VMI_OK)
	{
		g_pPlutoLogger->Write(LV_WARNING, "#VIA Created alpha surface for via overlay!");
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "#VIA Cannot create alpha surface for via overlay!");
		return false;
	}

	ddLock.dwFourCC = FOURCC_ALPHA;

	dwRet = VMI_DriverProc(&m_VMI_Info, LOCKSURFACE, (void *)&ddLock, NULL);
	if (dwRet == VMI_OK)
	{
		g_pPlutoLogger->Write(LV_WARNING, "#VIA Locked alpha surface for via overlay!");
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "#VIA Cannot lock alpha surface for via overlay!");
		return false;
	}

	m_lpAlphaSurface = ddLock.ALPDevice.lpALPOverlaySurface;
	memset(m_lpAlphaSurface, 0xFF, ulWidth * ulHeight);

	m_bOverlayInitialized = true;
	return true;
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::SetAlphaSurface()
{
	ALPHACTRL   AlphaCtrl;
	unsigned char dwRet = 0;

	AlphaCtrl.AlphaEnable = TRUE;
	AlphaCtrl.type = ALPHA_STREAM;

	return VMI_OK == VMI_DriverProc(&m_VMI_Info, SETALPHAWIN, (void *)&AlphaCtrl, NULL);
} 
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::UpdateAlphaSurface()
{
	RECTL rDest;
	rDest.left      = 0;
	rDest.top       = 0;
	rDest.right     = m_ulWidth;
	rDest.bottom    = m_ulHeight;

	if(VMI_OK != VMI_DriverProc(&m_VMI_Info, UPDATEALPHA, (void *)&rDest, NULL))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "#VIA Failed to update alpha surface!");
		return false;
	}

	return SetAlphaSurface();
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::Test_FillAlphaSurface()
{
	unsigned int XIndex=0,YIndex=0;

	for (YIndex=0; YIndex<m_ulHeight; YIndex++)
	{
		for (XIndex=0; XIndex<m_ulWidth; XIndex++)
		{
			if ((XIndex)>=((2*m_ulWidth)/3))
			{
				*(m_lpAlphaSurface+YIndex*m_ulWidth+XIndex)=0x6F;
			}
			else if ((XIndex)>=(m_ulWidth/3))
			{
				*(m_lpAlphaSurface+YIndex*m_ulWidth+XIndex)=0x1F;
			}
			else
			{
				*(m_lpAlphaSurface+YIndex*m_ulWidth+XIndex)=0x3F;
			}
		}
	}
} 
//-------------------------------------------------------------------------------------------------------