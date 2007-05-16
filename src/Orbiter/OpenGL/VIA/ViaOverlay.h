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
#ifndef __VIA_OVERLAY_H__
#define __VIA_OVERLAY_H__
//-------------------------------------------------------------------------------------------------------
extern "C" 
{
	#include "via_types.h"
	#include "viavid.h"
};
//-------------------------------------------------------------------------------------------------------
#include <list>
#include "DCE/Logger.h" 
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
	void FillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value, bool bMergeToScreen = false);

	/*
	* Special methods for alpha surface access
	*/
	void ScreenRenderer();
	void ShowPopup(int x, int y, int w, int h);
	void HidePopup(int x, int y, int w, int h);

	/*
	 * Suspend/resume overlay
	 */ 
	void SuspendOverlay();
	void ResumeOverlay();

	/*
	*  Debugging methods
	*/ 
	void DumpMask();

private:

	/*
	*  Screen mutex
	*/
	pluto_pthread_mutex_t m_ScreenMutex; 

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

	bool m_bOverlaySuspended;

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
	void InternalFillRectangleInAlphaMask(int x, int y, int w, int h, unsigned char value, bool bMergeToScreen = false);
};
//-------------------------------------------------------------------------------------------------------
#endif
