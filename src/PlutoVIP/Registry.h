//////////////////////////////////////////////////////////////////////
// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#include "PlutoRegistryKeyDef.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry  
{
public:
	BOOL IsOpen();
	void GetValue(CString szValueName, LPDWORD pType, LPBYTE pValue, LPDWORD pValueSize);
	BYTE* GetValueBinary(CString szValueName, LPBYTE pValue, LPDWORD pValueSize);
	DWORD GetValueDWORD(CString szValueName);
	CString GetValueString(CString szValueName);
	void SetValueBinary(CString szValue, const LPBYTE lpData, DWORD dwData);
	void SetValueDWORD(CString szValue, DWORD dwData);
	void SetValueString(CString szValue, CString szData);
	void SetValue(CString szValue, DWORD dwType, const LPBYTE lpData, DWORD dwData);
	LONG QueryValue(CString szValue, LPDWORD lpdwType, LPBYTE lpData, 
					LPDWORD lpdwData);
	LONG EnumerateValue(DWORD dwIndex, TCHAR* szName, 
						LPDWORD lpdwName, LPDWORD lpdwType, 
						LPBYTE lpData, LPDWORD lpdwData);
	LONG EnumerateKey(DWORD dwIndex, TCHAR* szName, LPDWORD lpdwName);
	void DeleteValue(CString szValue);
	void RemoveEntireKey(CString szSubKey);
	void DeleteKey(CString szSubKey);
	void CloseKey();
	HKEY CreateNewKey(HKEY hParent, CString szSubKey, LPDWORD lpdwDisposition);
	HKEY OpenKey(HKEY hParent, CString szSubKey);
	CRegistry();
	virtual ~CRegistry();

protected:
	HKEY m_Key;
};

#endif // _REGISTRY_H_
