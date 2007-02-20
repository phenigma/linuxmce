#ifndef __VIA_OVERLAY_H__
#define __VIA_OVERLAY_H__
//-------------------------------------------------------------------------------------------------------
extern "C" 
{
	#include "via_types.h"
	#include "viavid.h"
};

#include <list>
//-------------------------------------------------------------------------------------------------------
class AlphaMaskShapeItem
{
private:

	AlphaMaskShapeItem() {}

public:

	AlphaMaskShapeItem(int x, int y, int w, int h, const unsigned char *mask) :
		m_x(x), m_y(y), m_w(w), m_h(h), m_mask(mask) {}

	int m_x; 
	int m_y; 
	int m_w; 
	int m_h; 
	const unsigned char *m_mask;
};
//-------------------------------------------------------------------------------------------------------
class AlphaRectangleItem
{
private:

	AlphaRectangleItem() {}

public:

	AlphaRectangleItem(int x, int y, int w, int h, unsigned char value) :
		m_x(x), m_y(y), m_w(w), m_h(h), m_value(value) {}

	int m_x; 
	int m_y;
	int m_w;
	int m_h;
	unsigned char m_value;
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

	/*
	* Special methods for alpha surface access
	*/
	void ScreenRenderer();
	void ShowPopup(int x, int y, int w, int h);
	void HidePopup(int x, int y, int w, int h);

private:

	/*
	*  VMI data
	*/
	VMI_INFO_PARAM m_VMI_Info;
	unsigned char *m_lpAlphaSurface;
	int m_nWidth, m_nHeight;
	bool m_bOverlayInitialized;

	/*
	*  Popups state
	*/ 
	bool m_bHasPopups;
	unsigned char *m_ScreenMask;
	unsigned char *m_BufferMask;

	/*
	* VMI wrappers
	*/
	bool VMI_CreateConnection();
	bool CreateAlphaSurface(int nWidth, int nHeight);
	bool SetAlphaSurface();
	bool UpdateAlphaSurface();

	/*
	*  Internal methods
	*/ 
	void InternalApplyAlphaMask(int x, int y, int w, int h, const unsigned char *mask);
	void InternalFillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value);
};
//-------------------------------------------------------------------------------------------------------
#endif
