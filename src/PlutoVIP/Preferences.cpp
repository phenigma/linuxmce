// Preferences.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferences

IMPLEMENT_DYNAMIC(CPreferences, CPropertySheet)

CPreferences::CPreferences(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	//AddPage(&m_Introduction);
	AddPage(&m_PanelPosition);
	AddPage(&m_Database);
	AddPage(&m_Internet);
	AddPage(&m_Terminals);
}

CPreferences::CPreferences(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	//AddPage(&m_Introduction);
	AddPage(&m_PanelPosition);
	AddPage(&m_Database);
	AddPage(&m_Internet);
	AddPage(&m_Terminals);
}

CPreferences::~CPreferences()
{
}


BEGIN_MESSAGE_MAP(CPreferences, CPropertySheet)
	//{{AFX_MSG_MAP(CPreferences)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferences message handlers
