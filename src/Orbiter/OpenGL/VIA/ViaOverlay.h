#ifndef __VIA_OVERLAY_H__
#define __VIA_OVERLAY_H__
//-------------------------------------------------------------------------------------------------------
extern "C" 
{
	#include "via_types.h"
	#include "viavid.h"
};
//-------------------------------------------------------------------------------------------------------
class ViaOverlay
{
private:

	/*
	*  Private onstructor
	*/
	ViaOverlay();

	/*
	*  The one and only instance of ViaOverlay
	*/
	static ViaOverlay m_Instance;

public:

	/*
	*   Public destructor
	*/
	~ViaOverlay();

	/*
	*  Singleton access to its instance
	*/ 
	static ViaOverlay& Instance() { return m_Instance; }

	/*
	*   Notifications
	*/
	void WindowResized();
	void WindowCreated(unsigned long ulWidth, unsigned long ulHeight);
	void WorldChanged();

	/*
	*  Alpha surface access
	*/
	void ResetAlphaMask();
	void ApplyAlphaMask(int x, int y, int w, int h, const unsigned char *mask);
	void FillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value);

private:

	/*
	*  VMI data
	*/
	VMI_INFO_PARAM m_VMI_Info;
	unsigned char *m_lpAlphaSurface;
	int m_nWidth, m_nHeight;
	bool m_bOverlayInitialized;

	/*
	* VMI wrappers
	*/
	bool VMI_CreateConnection();
	bool CreateAlphaSurface(int nWidth, int nHeight);
	bool SetAlphaSurface();
	bool UpdateAlphaSurface();

	void Test_FillAlphaSurface();
};
//-------------------------------------------------------------------------------------------------------
#endif
