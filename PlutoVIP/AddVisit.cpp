// AddVisit.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "AddVisit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddVisit dialog


CAddVisit::CAddVisit(CWnd* pParent /*=NULL*/)
	: CDialog(CAddVisit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddVisit)
	m_Visit = 1;
	//}}AFX_DATA_INIT
}


void CAddVisit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddVisit)
	DDX_Text(pDX, IDC_VISIT, m_Visit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddVisit, CDialog)
	//{{AFX_MSG_MAP(CAddVisit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddVisit message handlers
