// VIPDesign.h : main header file for the VIPDesign application
//
#pragma once

#include "resource.h"       // main symbols

// CVIPDesignApp:
// See VIPDesign.cpp for the implementation of this class
//

class CVIPDesignApp : public CWinApp
{
public:
	CVIPDesignApp();

	pthread_t m_pthread_emulator;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CVIPDesignApp theApp;