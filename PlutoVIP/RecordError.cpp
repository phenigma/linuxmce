// RecordError.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "RecordError.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordError dialog


CRecordError::CRecordError(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordError::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecordError)
	//}}AFX_DATA_INIT
}


void CRecordError::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordError)
	DDX_Control(pDX, IDC_MAC_ADDRESS, m_MACAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecordError, CDialog)
	//{{AFX_MSG_MAP(CRecordError)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordError message handlers

BOOL CRecordError::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString FullMAC;
	FullMAC.Format("%s:%s:%s:%s:%s:%s", szMACAddress.Mid(0, 2),
								  szMACAddress.Mid(2, 2),
								  szMACAddress.Mid(4, 2),
								  szMACAddress.Mid(6, 2),
								  szMACAddress.Mid(8, 2),
								  szMACAddress.Mid(10, 2));
	CString temp;
	temp.Format("MAC Address:      <b>%s</b>", FullMAC.GetBuffer(0));
	m_MACAddress.SetWindowText(temp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
