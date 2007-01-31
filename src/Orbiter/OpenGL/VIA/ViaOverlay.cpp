#include "ViaOverlay.h"

#include "DCE/Logger.h"
using namespace DCE;
//-------------------------------------------------------------------------------------------------------
ViaOverlay ViaOverlay::m_Instance;
//-------------------------------------------------------------------------------------------------------
ViaOverlay::ViaOverlay() : m_lpAlphaSurface(NULL), m_nWidth(0), m_nHeight(0), m_bOverlayInitialized(false)
{
	memset(&m_VMI_Info, 0, sizeof(VMI_INFO_PARAM));
}
//-------------------------------------------------------------------------------------------------------
ViaOverlay::~ViaOverlay()
{

}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::WindowCreated(unsigned long nWidth, unsigned long nHeight)
{
	g_pPlutoLogger->Write(LV_STATUS, "#VIA ViaOverlay::WindowCreated");

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if(VMI_CreateConnection())
	{
		g_pPlutoLogger->Write(LV_WARNING, "#VIA Created connection to VIA drivers!");
		if(CreateAlphaSurface(nWidth, nHeight))
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
		if(UpdateAlphaSurface())
		{
			g_pPlutoLogger->Write(LV_WARNING, "#VIA Alpha surface updated!");
		}
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ResetAlphaMask()
{
	g_pPlutoLogger->Write(LV_STATUS, "#VIA Reseting alpha surface...");
	memset(m_lpAlphaSurface, 0xFF, m_nWidth * m_nHeight);
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::ApplyAlphaMask(int x, int y, int w, int h, const unsigned char *mask)
{
	if(x + w <= m_nWidth && x >= 0 && y >= 0 && y + h <= m_nHeight && NULL != mask)
	{
		g_pPlutoLogger->Write(LV_STATUS, "#VIA Applying alpha for %p (%d,%d,%d,%d) ...", mask, x, y, w, h);

		if(x == 0 && y == 0 && w == m_nWidth && h == m_nHeight)
			memcpy(m_lpAlphaSurface, mask, w * h);
		else
		{
			for(int i = 0; i < h; i++)
				memcpy(m_lpAlphaSurface + (y + i) * m_nWidth + x, mask + i * w, w);
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "#VIA Unabled to apply alpha for %p (%d,%d,%d,%d) !", mask, x, y, w, h);
	}
}
//-------------------------------------------------------------------------------------------------------
void ViaOverlay::FillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value)
{
	if(x + w <= m_nWidth && x >= 0 && y >= 0 && y + h <= m_nHeight)
	{
		g_pPlutoLogger->Write(LV_STATUS, "#VIA Filled rectangle in alpha mask (%d,%d,%d,%d), value %d...", 
			x, y, w, h, value);

		if(x == 0 && y == 0 && w == m_nWidth && h == m_nHeight)
			memset(m_lpAlphaSurface, value, w * h);
		else
		{
			for(int i = 0; i < h; i++)
				memset(m_lpAlphaSurface + (y + i) * m_nWidth + x, value, w);
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "#VIA Unabled to fill rectangle in alpha mask (%d,%d,%d,%d) value %d !", 
			x, y, w, h, value);
	}
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::VMI_CreateConnection()
{
	return VMI_OK == VMI_Create(&m_VMI_Info);
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::CreateAlphaSurface(int nWidth, int nHeight)
{
	DDSURFACEDESC ddSurfaceDesc;
	DDLOCK  ddLock;
	unsigned int dwRet = 0;

	ddSurfaceDesc.dwFourCC = FOURCC_ALPHA;
	ddSurfaceDesc.dwWidth = nWidth;
	ddSurfaceDesc.dwHeight = nHeight;

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
	memset(m_lpAlphaSurface, 0xFF, nWidth * nHeight);

	m_bOverlayInitialized = true;
	return true;
}
//-------------------------------------------------------------------------------------------------------
bool ViaOverlay::SetAlphaSurface()
{
	ALPHACTRL AlphaCtrl;
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
	rDest.right     = m_nWidth;
	rDest.bottom    = m_nHeight;

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
	int XIndex=0,YIndex=0;

	for (YIndex=0; YIndex<m_nHeight; YIndex++)
	{
		for (XIndex=0; XIndex<m_nWidth; XIndex++)
		{
			if ((XIndex)>=((2*m_nWidth)/3))
			{
				*(m_lpAlphaSurface+YIndex*m_nWidth+XIndex)=0x6F;
			}
			else if ((XIndex)>=(m_nWidth/3))
			{
				*(m_lpAlphaSurface+YIndex*m_nWidth+XIndex)=0x1F;
			}
			else
			{
				*(m_lpAlphaSurface+YIndex*m_nWidth+XIndex)=0x3F;
			}
		}
	}
} 
//-------------------------------------------------------------------------------------------------------
