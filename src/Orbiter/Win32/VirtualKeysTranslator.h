#ifndef __VIRTUAL_KEYS_TRANSLATOR_H__
#define __VIRTUAL_KEYS_TRANSLATOR_H__

#include "Windows.h"
//-----------------------------------------------------------------------------------------------------
#define VK_A 0x41
#define VK_C 0x43
#define VK_E 0x45
#define VK_T 0x54
#define VK_P 0x50
#define VK_L 0x4C
#define VK_R 0x52
#define VK_Z 0x5A
#define VK_W 0x57
//-----------------------------------------------------------------------------------------------------
bool TranslateVirtualKeys2PlutoKeys(UINT uMsg, WPARAM wParam, LPARAM lParam,
                               int& iPK_Button, bool& bShiftDown, bool& bAltDown, 
                               bool& bControlDown, bool& m_bCapsLock);
//-----------------------------------------------------------------------------------------------------
#endif
