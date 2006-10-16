#pragma once
//-----------------------------------------------------------------------------------------------------
#include "Wingdi.h"

#ifdef WINCE
#define LPDRAWTEXTPARAMS void *
#endif
//-----------------------------------------------------------------------------------------------------
LPWSTR ToWide(LPCSTR lpString, int cbString, int* sz)
{
	LPWSTR lpwString;

	// Determine required buffer
	*sz = MultiByteToWideChar(CP_UTF8, 0, lpString, cbString, 0, 0);
	if (*sz == 0) { return 0; }

	// Allocate memory
	lpwString = (LPWSTR)new WCHAR[*sz * sizeof(wchar_t)];
	if (lpwString == 0) { return 0; }

	// Convert utf-8 to wide character
	*sz = MultiByteToWideChar(CP_UTF8, 0, lpString, cbString, lpwString, *sz);
	if (*sz == 0) 
	{
		delete [] lpwString;
		return 0;
	}

	return lpwString;
}
//-----------------------------------------------------------------------------------------------------
int DrawTextExUTF8(HDC hdc, LPCSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{
	int sz;
	LPWSTR lpwchText;
	BOOL retVal;

	// Correction for null
	int np = (cchText == -1) ? 1 : 0;

	lpwchText = ToWide(lpchText, cchText, &sz);

#ifdef WINCE
	retVal = DrawText(hdc, lpwchText, sz - np, lprc, dwDTFormat);
#else
	// Do unicode DrawTextExW
	retVal = DrawTextExW(hdc, lpwchText, sz - np, lprc, dwDTFormat, lpDTParams);
#endif

	// Free buffer
	delete [] lpwchText;

	return retVal;
}