// BondedDevices.h: interface for the CBondedDevices class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_)
#define AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtIfDefinitions.h"

class CBondedDeviceInfo
{
public:
	BOOL IsUnknown() const;
	void Init();
	void DeepCopy( const CBondedDeviceInfo* di );
	BOOL IsCellphone() const;
	CBondedDeviceInfo();

	CString textaddr;
	BD_ADDR addr;
	BD_NAME name;
	DEV_CLASS devclass;
};

class CBondedDevices  
{
public:
	UINT GetCount();
	void Refresh();
	CBondedDevices();
	virtual ~CBondedDevices();
	const CBondedDeviceInfo* GetAt( UINT index );
	static BOOL ConvertAddr( const TCHAR* text, BD_ADDR addr );

protected:
	CList<CBondedDeviceInfo*,CBondedDeviceInfo*> m_BondedDevList;
	static BOOL hexPair( const TCHAR* text, BYTE* val );
	void DeleteAll();
};

#endif // !defined(AFX_BONDEDDEVICES_H__EE0E8196_D3F0_4CD6_9CC3_E7B97D2F8DD2__INCLUDED_)
