// DlgResample.cpp : implementation file
//

#include "stdafx.h"
#include "demo.h"
#include "DlgResample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
// DlgResample dialog


DlgResample::DlgResample(CWnd* pParent /*=NULL*/)
	: CDialog(DlgResample::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgResample)
	m_factor = 0.0f;
	m_fast = FALSE;
	//}}AFX_DATA_INIT
}


void DlgResample::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgResample)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_canc);
	DDX_Text(pDX, IDC_EDIT1, m_factor);
	DDX_Check(pDX, IDC_CHECK1, m_fast);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgResample, CDialog)
	//{{AFX_MSG_MAP(DlgResample)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
// DlgResample message handlers

BOOL DlgResample::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ok.SetIcon(IDI_G,BS_LEFT);
	m_canc.SetIcon(IDI_R,BS_LEFT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
