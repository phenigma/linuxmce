// WindowsInstaller.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

bool HandleCancel(CDialog *pDialog);

// CWindowsInstallerApp:
// See WindowsInstaller.cpp for the implementation of this class
//

class CWindowsInstallerApp : public CWinApp
{
public:
	CWindowsInstallerApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CWindowsInstallerApp theApp;