#ifndef _UTILITY_FUNC_H_
#define _UTILITY_FUNC_H_

#define WS_EX_LAYERED           0x00080000    

#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002

BOOL MakeWindowTransparent(HWND hWnd, UCHAR factor);
#endif	//_UTILITY_FUNC_H_
